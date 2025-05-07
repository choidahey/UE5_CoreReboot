// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "CharacterController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


// Sets default values
ABasePlayer::ABasePlayer()
{
	//Set Tick
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
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
}

// Called when the game starts or when spawned
void ABasePlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABasePlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (ACharacterController* MyController = Cast<ACharacterController>(PlayerController))
			{
				Subsystem->AddMappingContext(MyController->DefaultMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ABasePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABasePlayer::Move(const FInputActionValue& Value)
{
	if (!Controller) return;
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

void ABasePlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookInput=Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ABasePlayer::StartJump(const FInputActionValue& Value)
{
	// Jump 함수는 Character가 기본 제공
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ABasePlayer::StopJump(const FInputActionValue& Value)
{
	// StopJumping 함수도 Character가 기본 제공
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ABasePlayer::StartSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
	}
}

void ABasePlayer::StopSprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
	}
}

// Called to bind functionality to input
void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		if (ACharacterController* MyController=Cast<ACharacterController>(GetController()))
		{
			// Moving
			EnhancedInputComponent->BindAction(MyController->MoveAction, ETriggerEvent::Triggered, this, &ABasePlayer::Move);
			//Looking
			EnhancedInputComponent->BindAction(MyController->LookAction,ETriggerEvent::Triggered, this, &ABasePlayer::Look);
			//Sprinting
			EnhancedInputComponent->BindAction(MyController->SprintAction, ETriggerEvent::Triggered, this, &ABasePlayer::StartSprint);
			EnhancedInputComponent->BindAction(MyController->SprintAction, ETriggerEvent::Completed, this, &ABasePlayer::StopSprint);
			// Jumping
			EnhancedInputComponent->BindAction(MyController->JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
			EnhancedInputComponent->BindAction(MyController->JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

