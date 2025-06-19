#include "PlayerCharacter.h"
#include "AlsCameraComponent.h"
#include "AlsCharacterMovementComponent.h"
#include "CR4S.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CharacterController.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Gimmick/Components/InteractionComponent.h"
#include "Character/Components/GridDetectionComponent.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "UI/InGame/DefaultInGameWidget.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Utility/AlsVector.h"
#include "NavigationInvokerComponent.h"
#include "Character/Components/PlayerInputBufferComponent.h"
#include "Character/Components/PlayerInputBufferComponent.h"
#include "Character/Components/WeaponTraceComponent.h"
#include "Character/Weapon/BaseTool.h"
#include "Character/Weapon/PlayerTool.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Tests/AutomationCommon.h"
#include "UI/InGame/CharacterEnvironmentStatusWidget.h"


APlayerCharacter::APlayerCharacter()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});

	VisibleMesh=CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VisibleMesh"));
	VisibleMesh->SetupAttachment(GetMesh());
	
	WeaponTrace=CreateDefaultSubobject<UWeaponTraceComponent>(TEXT("WeaponTrace"));

	PlayerInputBuffer=CreateDefaultSubobject<UPlayerInputBufferComponent>(TEXT("PlayerInputBuffer"));
	
	Status=CreateDefaultSubobject<UPlayerCharacterStatusComponent>(TEXT("Status"));

	Interaction=CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
	GridDetection = CreateDefaultSubobject<UGridDetectionComponent>(TEXT("GridDetection"));
	EnvironmentalStatus = CreateDefaultSubobject<UEnvironmentalStatusComponent>(TEXT("EnvironmentalStatus"));

	NavGenerationRadius = 2000.0f;
	NavRemovalRadius = 2500.0f;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);

	PlayerInventory = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("PlayerInventory"));
}

void APlayerCharacter::OnDeath()
{
	SetOverlayMode(AlsOverlayModeTags::Default);
	StartRagdolling();
	
	APlayerController* PC=Cast<APlayerController>(GetController());
	if (!CR4S_ENSURE(LogHong1,PC)) return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!CR4S_ENSURE(LogHong1,InputSubsystem)) return;

	InputSubsystem->RemoveMappingContext(InputMappingContext);
}

void APlayerCharacter::SetCurrentToolByTag(const FGameplayTag& ToolTag)
{
	if (!CurrentTool||CurrentTool->GetGameplayTag().MatchesTagExact(ToolTag)) return;
	
	CurrentTool->SetGameplayTag(ToolTag);
	
	SetOverlayMode(ToolTag);
}

void APlayerCharacter::InitializeCurrentTool()
{
	if (CR4S_ENSURE(LogHong1,CurrentTool))
	{
		CurrentTool->Destroy();
		CurrentTool=nullptr;
	}
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator=this;
	SpawnParams.Owner=this;
	APlayerTool* NewTool=GetWorld()->SpawnActor<APlayerTool>(SpawnParams);
	const FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepRelative,
		true
	);
	if (!CR4S_ENSURE(LogHong1,NewTool)) return;
	
	CurrentTool=NewTool;
	CurrentTool->AttachToComponent(VisibleMesh,AttachRules,"Tool");
	
	if (!CR4S_ENSURE(LogHong1,PlayerInputBuffer)||!CR4S_ENSURE(LogHong1,WeaponTrace)) return;
	PlayerInputBuffer->SetCurrentTool(CurrentTool);
	WeaponTrace->SetCurrentTool(CurrentTool);
	CurrentTool->Initialize(this);
}

void APlayerCharacter::InitializeWidgets()
{
	if (ACharacterController* CurrentController=Cast<ACharacterController>(GetController()))
	{
		if (ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(CurrentController->GetHUD()))
		{
			if (UDefaultInGameWidget* InGameWidget=CurrentHUD->GetInGameWidget())
			{
				Status->OnHPChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateHPWidget);
				Status->OnResourceChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateResourceWidget);
				Status->OnHungerChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateHungerWidget);

				InGameWidget->InitializeStatusWidget(Status,false);
				
				if (UCharacterEnvironmentStatusWidget* EnvironmentWidget=InGameWidget->GetEnvironmentStatusWidget())
				{
					if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
					
					EnvironmentalStatus->OnTemperatureChanged.AddDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
					EnvironmentalStatus->OnHumidityChanged.AddDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);

					if (!CR4S_ENSURE(LogHong1,Status)) return;
					
					Status->OnColdThresholdChanged.AddDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateColdThreshold);
					Status->OnHeatThresholdChanged.AddDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateHeatThreshold);
					Status->OnHumidityThresholdChanged.AddDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateHumidityThreshold);
					EnvironmentWidget->InitializeWidget(this);
				}
			}
		}
	}
}

void APlayerCharacter::DisconnectWidgets()
{
	if (ACharacterController* CurrentController=Cast<ACharacterController>(GetController()))
	{
		if (ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(CurrentController->GetHUD()))
		{
			if (UDefaultInGameWidget* InGameWidget=CurrentHUD->GetInGameWidget())
			{
				Status->OnHPChanged.RemoveAll(InGameWidget);
				Status->OnResourceChanged.RemoveAll(InGameWidget);
				Status->OnHungerChanged.RemoveAll(InGameWidget);
				InGameWidget->InitializeStatusWidget(Status,false);
				
				if (UCharacterEnvironmentStatusWidget* EnvironmentWidget=InGameWidget->GetEnvironmentStatusWidget())
				{
					if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
					
					EnvironmentalStatus->OnTemperatureChanged.RemoveDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
					EnvironmentalStatus->OnHumidityChanged.RemoveDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);

					if (!CR4S_ENSURE(LogHong1,Status)) return;
					
					Status->OnColdThresholdChanged.RemoveDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateColdThreshold);
					Status->OnHeatThresholdChanged.RemoveDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateHeatThreshold);
					Status->OnHumidityThresholdChanged.RemoveDynamic(EnvironmentWidget,&UCharacterEnvironmentStatusWidget::UpdateHumidityThreshold);
				}
			}
		}
	}
}

void APlayerCharacter::NotifyControllerChanged()
{
	const auto* PreviousPlayer{Cast<APlayerController>(PreviousController)};
	if (IsValid(PreviousPlayer))
	{
		auto* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PreviousPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}

	auto* NewPlayer{Cast<APlayerController>(GetController())};
	if (IsValid(NewPlayer))
	{
		NewPlayer->InputYawScale_DEPRECATED = 1.0f;
		NewPlayer->InputPitchScale_DEPRECATED = 1.0f;
		NewPlayer->InputRollScale_DEPRECATED = 1.0f;

		auto* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(NewPlayer->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			FModifyContextOptions Options;
			Options.bNotifyUserSettings = true;

			InputSubsystem->AddMappingContext(InputMappingContext, PlayerCharacterSettings.MappingContextPriority, Options);
		}
	}
	
	Super::NotifyControllerChanged();
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Status->AddCurrentHP(-DamageAmount);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Binding Delegate Functions and Set up Widget
	InitializeWidgets();

	InitializeCurrentTool();

	if (!CR4S_ENSURE(LogHong1,PlayerCharacterSettingsDataAsset)) return;
	PlayerCharacterSettings=PlayerCharacterSettingsDataAsset->PlayerCharacterSettings;

	if (!CR4S_ENSURE(LogHong1,Status)) return;
	Status->OnDeathState.AddUObject(this,&APlayerCharacter::OnDeath);

	if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
	EnvironmentalStatus->OnTemperatureChanged.AddDynamic(Status,&UBaseStatusComponent::HandleTemperatureChanged);
	EnvironmentalStatus->OnHumidityChanged.AddDynamic(Status,&UBaseStatusComponent::HandleHumidityChanged);
}

void APlayerCharacter::CalcCamera(const float DeltaTime, FMinimalViewInfo& ViewInfo)
{
	if (Camera->IsActive())
	{
		Camera->GetViewInfo(ViewInfo);
		return;
	}

	Super::CalcCamera(DeltaTime, ViewInfo);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* Input)
{
	Super::SetupPlayerInputComponent(Input);

	auto* EnhancedInput{Cast<UEnhancedInputComponent>(Input)};
	if (IsValid(EnhancedInput))
	{
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(LookMouseAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnLookMouse);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnLook);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnMove);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnSprint);
		EnhancedInput->BindAction(WalkAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnWalk);
		EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnCrouch);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnJump);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Canceled, this, &ThisClass::Input_OnAim);
		EnhancedInput->BindAction(RagdollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRagdoll);
		EnhancedInput->BindAction(RollAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRoll);
		EnhancedInput->BindAction(RotationModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnRotationMode);
		EnhancedInput->BindAction(ViewModeAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnViewMode);
		EnhancedInput->BindAction(SwitchShoulderAction, ETriggerEvent::Triggered, this, &ThisClass::Input_OnSwitchShoulder);
		EnhancedInput->BindAction(AttackAction,ETriggerEvent::Triggered,this,&ThisClass::Input_OnAttack);
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	SetOverlayMode(AlsOverlayModeTags::Default);

	Interaction->StartDetectProcess();
	
	InitializeWidgets();
	Status->SetIsUnPossessed(false);
	
}

void APlayerCharacter::UnPossessed()
{
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}
	
	SetOverlayMode(OverlayMode::Mounted);
	DisconnectWidgets();
	Status->SetIsUnPossessed(true);
	Super::UnPossessed();
}

void APlayerCharacter::Input_OnLookMouse(const FInputActionValue& ActionValue)
{
	const FVector2f Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpMouseSensitivity);
	AddControllerYawInput(Value.X * LookRightMouseSensitivity);
}

void APlayerCharacter::Input_OnLook(const FInputActionValue& ActionValue)
{
	const FVector2f Value{ActionValue.Get<FVector2D>()};

	AddControllerPitchInput(Value.Y * LookUpRate);
	AddControllerYawInput(Value.X * LookRightRate);
}

void APlayerCharacter::Input_OnMove(const FInputActionValue& ActionValue)
{
	const auto Value{UAlsVector::ClampMagnitude012D(ActionValue.Get<FVector2D>())};

	const auto ForwardDirection{UAlsVector::AngleToDirectionXY(UE_REAL_TO_FLOAT(GetViewState().Rotation.Yaw))};
	const auto RightDirection{UAlsVector::PerpendicularCounterClockwiseXY(ForwardDirection)};

	AddMovementInput(ForwardDirection * Value.Y + RightDirection * Value.X);
}

void APlayerCharacter::Input_OnSprint(const FInputActionValue& ActionValue)
{
	const bool bIsSprinting=ActionValue.Get<bool>() && !GetLastMovementInputVector().IsNearlyZero();
	
	SetDesiredGait(bIsSprinting ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
	if (bIsSprinting)
	{
		Status->StartSprint();
	}
	else
	{
		Status->StopSprint();
	}
}

void APlayerCharacter::Input_OnWalk()
{
	if (GetDesiredGait() == AlsGaitTags::Walking)
	{
		SetDesiredGait(AlsGaitTags::Running);
	}
	else if (GetDesiredGait() == AlsGaitTags::Running)
	{
		SetDesiredGait(AlsGaitTags::Walking);
	}
}

void APlayerCharacter::Input_OnCrouch()
{
	if (GetDesiredStance() == AlsStanceTags::Standing)
	{
		SetDesiredStance(AlsStanceTags::Crouching);
	}
	else if (GetDesiredStance() == AlsStanceTags::Crouching)
	{
		SetDesiredStance(AlsStanceTags::Standing);
	}
}

void APlayerCharacter::Input_OnJump(const FInputActionValue& ActionValue)
{
	if (ActionValue.Get<bool>())
	{
		if (StopRagdolling())
		{
			return;
		}

		if (StartMantlingGrounded())
		{
			return;
		}

		if (GetStance() == AlsStanceTags::Crouching)
		{
			SetDesiredStance(AlsStanceTags::Standing);
			return;
		}

		Jump();
	}
	else
	{
		StopJumping();
	}
}

void APlayerCharacter::Input_OnAim(const FInputActionValue& ActionValue)
{
	SetDesiredAiming(ActionValue.Get<bool>());
}

void APlayerCharacter::Input_OnRagdoll()
{
	if (!StopRagdolling())
	{
		StartRagdolling();
	}
}

void APlayerCharacter::Input_OnRoll()
{
	static constexpr auto PlayRate{1.3f};
	if (!CR4S_ENSURE(LogHong1,Status->HasEnoughResourceForRoll()
		&& LocomotionAction!=AlsLocomotionActionTags::Rolling)) return;
		
	StartRolling(PlayRate);
	Status->ConsumeResourceForRoll();
}

void APlayerCharacter::Input_OnRotationMode()
{
	SetDesiredRotationMode(GetDesiredRotationMode() == AlsRotationModeTags::VelocityDirection
		                       ? AlsRotationModeTags::ViewDirection
		                       : AlsRotationModeTags::VelocityDirection);
}

void APlayerCharacter::Input_OnViewMode()
{
	SetViewMode(GetViewMode() == AlsViewModeTags::ThirdPerson ? AlsViewModeTags::FirstPerson : AlsViewModeTags::ThirdPerson);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void APlayerCharacter::Input_OnSwitchShoulder()
{
	Camera->SetRightShoulder(!Camera->IsRightShoulder());
}

void APlayerCharacter::Input_OnAttack()
{
	if (!CR4S_ENSURE(LogHong1,CurrentTool)
		||!CR4S_ENSURE(LogHong1,(PlayerInputBuffer->CheckInputQueue(EInputType::Attack))))
	{
		return;
	}
	CurrentTool->OnAttack();
}

void APlayerCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}
