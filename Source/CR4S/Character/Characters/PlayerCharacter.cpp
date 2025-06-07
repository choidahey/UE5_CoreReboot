#include "PlayerCharacter.h"
#include "AlsCameraComponent.h"
#include "AlsCharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/CharacterController.h"
#include "Character/Components/CombatComponent.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Gimmick/Components/InteractionComponent.h"
#include "UI/InGame/DefaultInGameWidget.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Utility/AlsVector.h"
#include "NavigationInvokerComponent.h"


APlayerCharacter::APlayerCharacter()
{
	Camera = CreateDefaultSubobject<UAlsCameraComponent>(FName{TEXTVIEW("Camera")});
	Camera->SetupAttachment(GetMesh());
	Camera->SetRelativeRotation_Direct({0.0f, 90.0f, 0.0f});

	VisibleMesh=CreateDefaultSubobject<USkeletalMeshComponent>(FName{TEXTVIEW("VisibleMesh")});
	VisibleMesh->SetupAttachment(GetMesh());
	
	OverlaySkeletalMesh=CreateDefaultSubobject<USkeletalMeshComponent>(FName{TEXTVIEW("OverlaySkeletalMesh")});
	OverlaySkeletalMesh->SetupAttachment(VisibleMesh);
	
	OverlayStaticMesh=CreateDefaultSubobject<UStaticMeshComponent>(FName{TEXTVIEW("OverlayStaticMesh")});
	OverlayStaticMesh->SetupAttachment(VisibleMesh);
	
	Combat=CreateDefaultSubobject<UCombatComponent>(FName{TEXTVIEW("Combat")});

	Status=CreateDefaultSubobject<UPlayerCharacterStatusComponent>(FName{TEXTVIEW("Status")});

	Interaction=CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));


	NavGenerationRadius = 2000.0f;
	NavRemovalRadius = 2500.0f;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
}

void APlayerCharacter::SetToolStaticMesh(UStaticMesh* InMesh)
{
	ToolStaticMesh=InMesh;
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
				Status->OnResourceChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateEnergyWidget);
				Status->OnHungerChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateHungerWidget);

				InGameWidget->InitializeStatusWidget(Status,false);
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

			InputSubsystem->AddMappingContext(InputMappingContext, 0, Options);
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
		EnhancedInput->BindAction(AttackAction,ETriggerEvent::Triggered,Combat.Get(),&UCombatComponent::Input_OnAttack);
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	const FGameplayTag CurrentTag=FGameplayTag::RequestGameplayTag((FName("Als.OverlayMode.Default")));
	SetOverlayMode(CurrentTag);

	Interaction->StartDetectProcess();
	
	InitializeWidgets();
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
	const FGameplayTag CurrentTag=FGameplayTag::RequestGameplayTag((FName("Als.OverlayMode.Mounted")));
	SetOverlayMode(CurrentTag);
	DisconnectWidgets();
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
	SetDesiredGait(ActionValue.Get<bool>() ? AlsGaitTags::Sprinting : AlsGaitTags::Running);
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

	StartRolling(PlayRate);
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

void APlayerCharacter::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation)
{
	if (Camera->IsActive())
	{
		Camera->DisplayDebug(Canvas, DisplayInfo, VerticalLocation);
	}

	Super::DisplayDebug(Canvas, DisplayInfo, Unused, VerticalLocation);
}
