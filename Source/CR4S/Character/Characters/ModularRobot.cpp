// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularRobot.h"

#include "CR4S.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerCharacter.h"
#include "CR4S/Character/CharacterController.h"
#include "Camera/CameraComponent.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Components/RobotWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Character/Components/GridDetectionComponent.h"
#include "Character/Components/EnvironmentalStatusComponent.h"
#include "Character/Components/InputBufferComponent.h"
#include "Character/Components/RobotInputBufferComponent.h"
#include "Gimmick/Components/InteractionComponent.h"
#include "Inventory/Components/RobotInventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/InGame/CharacterEnvironmentStatusWidget.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Utility/DataLoaderSubsystem.h"


// Sets default values
AModularRobot::AModularRobot()
{
	//Set Tick
	PrimaryActorTick.bCanEverTick = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//Set Mesh option
	if (IsValid(GetMesh()))
	{
		GetMesh()->SetRelativeLocation_Direct({0.0f, 0.0f, -92.0f});
		GetMesh()->SetRelativeRotation_Direct({0.0f, -90.0f, 0.0f});

		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickMontagesWhenNotRendered;
		GetMesh()->bEnableUpdateRateOptimizations = false;
	}
	
	// Don't rotate when the controller rotates.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1200.0f; // The camera follows at this distance behind the character
	CameraBoom->SetWorldRotation((FRotator(-15, 0, 0)));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Status=CreateDefaultSubobject<UModularRobotStatusComponent>(TEXT("Status"));
	
	//InteractableComponent
	InteractComp=CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractComp"));

	WeaponManager=CreateDefaultSubobject<URobotWeaponComponent>(TEXT("WeaponManager"));

	InputBuffer=CreateDefaultSubobject<URobotInputBufferComponent>(TEXT("RobotInputBuffer"));
	
	RobotInventoryComponent = CreateDefaultSubobject<URobotInventoryComponent>(TEXT("RobotInventoryComponent"));

	EnvironmentalStatus=CreateDefaultSubobject<UEnvironmentalStatusComponent>(TEXT("EnvironmentalStatus"));
}

void AModularRobot::TakeStun_Implementation(const float StunAmount)
{
	 Status->AddStun(StunAmount);
}

void AModularRobot::SetInputEnable(const bool bEnableInput)
{
	APlayerController* PC=Cast<APlayerController>(GetController());
	if (!CR4S_ENSURE(LogHong1,PC)) return;

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!CR4S_ENSURE(LogHong1,InputSubsystem)) return;
	
	if (bEnableInput)
	{
		InputSubsystem->AddMappingContext(InputMappingContext,RobotSettings.MappingContextPriority);
	}
	else
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
	}
}

void AModularRobot::OnDeath()
{
	APlayerCharacter* PC=MountedCharacter;
	if (!CR4S_ENSURE(LogHong1,PC)) return;

	AController* CachedController=GetController();
	if (!CR4S_ENSURE(LogHong1,CachedController)) return;
	
	UnMountRobot();
	UGameplayStatics::ApplyDamage(
		PC,
		FLT_MAX,
		CachedController,
		this,
		UDamageType::StaticClass()
	);
}

void AModularRobot::LoadDataFromDataLoader()
{
	UGameInstance* GI=GetGameInstance();
	if (!CR4S_ENSURE(LogHong1,GI)) return;
	
	UDataLoaderSubsystem* Loader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!CR4S_ENSURE(LogHong1,Loader)) return;

	Loader->LoadRobotSettingsData(RobotSettings);
	
}

void AModularRobot::MountRobot(AActor* InActor)
{
	if (!IsValid(InActor)) return;

	ACharacter* PreviousCharacter=Cast<ACharacter>(InActor);
	if (IsValid(PreviousCharacter))
	{
		MountedCharacter=Cast<APlayerCharacter>(PreviousCharacter);
		RobotInventoryComponent->UpdatePlayerInventoryComponent(MountedCharacter);
		PreviousCharacter->SetActorEnableCollision(false);
		PreviousCharacter->SetActorTickEnabled(false);

		FAttachmentTransformRules AttachRule(
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepWorld,
			false
		);
		PreviousCharacter->AttachToComponent(
			GetMesh(),
			AttachRule,
			RobotSettings.MountSocketName
		);
	}
	AController* InController=Cast<AController>(PreviousCharacter->GetController());
	
	if (!IsValid(InController)) return;
	
	InController->UnPossess();
	InController->Possess(this);
	Status->StartConsumeEnergy();
}

void AModularRobot::UnMountRobot()
{
	ACharacter* NextCharacter=MountedCharacter.Get();
	if (IsValid(NextCharacter))
	{
		NextCharacter->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		FVector UnMountOffset=GetActorForwardVector()*RobotSettings.UnMountLocation;
		FVector DropLocation=GetActorLocation()+UnMountOffset;
		NextCharacter->TeleportTo(DropLocation,NextCharacter->GetActorRotation(),false,true);
		
		NextCharacter->SetActorEnableCollision(true);
		NextCharacter->SetActorTickEnabled(true);
	}

	if (AController* CurrentController=GetController())
	{
		CurrentController->UnPossess();
		if (IsValid(NextCharacter))
		{
			CurrentController->Possess(NextCharacter);
		}
	}
	MountedCharacter=nullptr;
	RobotInventoryComponent->UpdatePlayerInventoryComponent(MountedCharacter);
	Status->StopConsumeEnergy();
}

void AModularRobot::InitializeWidgets()
{
	if (ACharacterController* CurrentController=Cast<ACharacterController>(GetController()))
	{
		if (ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(CurrentController->GetHUD()))
		{
			if (UDefaultInGameWidget* InGameWidget=CurrentHUD->GetInGameWidget())
			{
				Status->OnHPChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateHPWidget);
				Status->OnResourceChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateResourceWidget);
				Status->OnEnergyChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateEnergyWidget);
				Status->OnStunChanged.AddUObject(InGameWidget,&UDefaultInGameWidget::UpdateStunWidget);
				
				InGameWidget->InitializeStatusWidget(Status,true);
				
				if (UCharacterEnvironmentStatusWidget* EnvironmentWidget=InGameWidget->GetEnvironmentStatusWidget())
				{
					if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
					
					EnvironmentalStatus->OnTemperatureChanged.AddDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
					EnvironmentalStatus->OnHumidityChanged.AddDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);
					EnvironmentWidget->InitializeWidget(this);
				}
			}
		}
	}
}

void AModularRobot::DisconnectWidgets()
{
	if (ACharacterController* CurrentController=Cast<ACharacterController>(GetController()))
	{
		if (ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(CurrentController->GetHUD()))
		{
			if (UDefaultInGameWidget* InGameWidget=CurrentHUD->GetInGameWidget())
			{
				Status->OnHPChanged.RemoveAll(InGameWidget);
				Status->OnResourceChanged.RemoveAll(InGameWidget);
				Status->OnEnergyChanged.RemoveAll(InGameWidget);
				Status->OnStunChanged.RemoveAll(InGameWidget);

				if (UCharacterEnvironmentStatusWidget* EnvironmentWidget=InGameWidget->GetEnvironmentStatusWidget())
				{
					if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
					
					EnvironmentalStatus->OnTemperatureChanged.RemoveDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnTemperatureChanged);
					EnvironmentalStatus->OnHumidityChanged.RemoveDynamic(EnvironmentWidget, &UCharacterEnvironmentStatusWidget::OnHumidityChanged);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void AModularRobot::BeginPlay()
{
	Super::BeginPlay();

	LoadDataFromDataLoader();
	
	if (InteractComp)
	{
		InteractComp->OnTryInteract.AddUniqueDynamic(this,&AModularRobot::MountRobot);
	}
	if (Status)
	{
		Status->OnDeathState.AddUObject(this,&AModularRobot::OnDeath);
	}
	
	InitializeWidgets();

	if (!CR4S_ENSURE(LogHong1,EnvironmentalStatus)) return;
	EnvironmentalStatus->OnTemperatureChanged.AddDynamic(Status,&UBaseStatusComponent::HandleTemperatureChanged);
	EnvironmentalStatus->OnHumidityChanged.AddDynamic(Status,&UBaseStatusComponent::HandleHumidityChanged);
}

void AModularRobot::NotifyControllerChanged()
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

			InputSubsystem->AddMappingContext(InputMappingContext, RobotSettings.MappingContextPriority, Options);
		}
	}

	Super::NotifyControllerChanged();
}

void AModularRobot::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	InitializeWidgets();
}

void AModularRobot::UnPossessed()
{
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem{ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer())};
		if (IsValid(InputSubsystem))
		{
			InputSubsystem->RemoveMappingContext(InputMappingContext);
		}
	}
	DisconnectWidgets();
	Super::UnPossessed();
}

// Called every frame
void AModularRobot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AModularRobot::Input_Move(const FInputActionValue& Value)
{
	if (!Controller||!Status->IsRobotActive()) return;
	//Move Logic
	// input is a Vector2D
	FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(),MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(),MoveInput.Y);
	}
}

void AModularRobot::Input_Look(const FInputActionValue& Value)
{
	if (!CR4S_ENSURE(LogHong1,Status->IsRobotActive())) return;
	FVector2D LookInput=Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AModularRobot::Input_StartJump(const FInputActionValue& Value)
{
	if (!CR4S_ENSURE(LogHong1,Status->IsRobotActive())) return;
	// Jump 함수는 Character가 기본 제공
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void AModularRobot::Input_StopJump(const FInputActionValue& Value)
{
	if (!CR4S_ENSURE(LogHong1,Status->IsRobotActive())) return;
	// StopJumping 함수도 Character가 기본 제공
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void AModularRobot::Input_Dash(const FInputActionValue& Value)
{
	if (bIsDashing||!Status->HasEnoughResourceForRoll()||!Status->IsRobotActive()) return;

	bIsDashing = true;
	
	FVector LastInput=GetLastMovementInputVector();
	FVector ForwardVector=GetActorForwardVector();
	FVector DashDirection=LastInput.IsNearlyZero()?ForwardVector:LastInput.GetSafeNormal();
	
	FVector LaunchVelocity=DashDirection*RobotSettings.DashStrength;
	Status->ConsumeResourceForRoll();
	LaunchCharacter(LaunchVelocity,true,false);
	GetWorldTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&AModularRobot::ResetDashCooldown,
		RobotSettings.DashCooldown,
		false
	);
}

void AModularRobot::ResetDashCooldown()
{
	bIsDashing = false;
}


// Called to bind functionality to input
void AModularRobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (ACharacterController* MyController=Cast<ACharacterController>(GetController()))
		{
			// Moving
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AModularRobot::Input_Move);
			//Looking
			EnhancedInputComponent->BindAction(LookAction,ETriggerEvent::Triggered, this, &AModularRobot::Input_Look);
			//Dash
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &AModularRobot::Input_Dash);
			// Jump
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AModularRobot::Input_StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AModularRobot::Input_StopJump);
			//Attack
			EnhancedInputComponent->BindAction(Attack1Action,ETriggerEvent::Triggered,WeaponManager.Get(),&URobotWeaponComponent::Input_OnAttackLeftArm);
			EnhancedInputComponent->BindAction(Attack2Action,ETriggerEvent::Triggered,WeaponManager.Get(),&URobotWeaponComponent::Input_OnAttackRightArm);
			EnhancedInputComponent->BindAction(Attack3Action,ETriggerEvent::Triggered,WeaponManager.Get(),&URobotWeaponComponent::Input_OnAttackLeftShoulder);
			EnhancedInputComponent->BindAction(Attack4Action,ETriggerEvent::Triggered,WeaponManager.Get(),&URobotWeaponComponent::Input_OnAttackRightShoulder);
			
		}
	}
}

float AModularRobot::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	Status->AddCurrentHP(DamageAmount);
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
