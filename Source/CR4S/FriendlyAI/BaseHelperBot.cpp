#include "BaseHelperBot.h"
#include "BaseHelperBot.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Controller/HelperBotAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperBotStatsSubsystem.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "Components/SplineComponent.h" 
#include "UI/HelperBotInfoWidget.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UI/InGame/SurvivalHUD.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/PointLightComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "Particles/ParticleSystemComponent.h"


ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->SetInteractionText(FText::FromString("자동화 로봇"));

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));

	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvokerComponent"));
	

	LeftEyeWorkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LeftEyeWorkSpline"));
	LeftEyeWorkSpline->SetupAttachment(RootComponent);

	RightEyeWorkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RightEyeWorkSpline"));
	RightEyeWorkSpline->SetupAttachment(RootComponent);

	LeftEyeWorkVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftEyeWorkVFX"));
	LeftEyeWorkVFXComponent->SetupAttachment(LeftEyeWorkSpline);
	LeftEyeWorkVFXComponent->SetAutoActivate(false);
	LeftEyeWorkVFXComponent->SetVisibility(false);

	RightEyeWorkVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightEyeWorkVFX"));
	RightEyeWorkVFXComponent->SetupAttachment(RightEyeWorkSpline);
	RightEyeWorkVFXComponent->SetAutoActivate(false);
	RightEyeWorkVFXComponent->SetVisibility(false);

	LeftEyeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LeftEyeLight"));
	LeftEyeLight->SetupAttachment(GetMesh(), TEXT("LeftEye"));

	RightEyeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RightEyeLight"));
	RightEyeLight->SetupAttachment(GetMesh(), TEXT("RightEye"));

	WorkTargetParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WorkTargetParticle"));
	WorkTargetParticle->SetAutoActivate(false);
}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();

	LoadStats();
	UpdateStateVisualEffects();
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetAvoidanceEnabled(true);
	}

	if (InteractableComp)
	{
		InteractableComp->OnTryInteract.AddUniqueDynamic(this, &ABaseHelperBot::HandleInteract);
		InteractableComp->OnDetectionStateChanged.AddUniqueDynamic(this, &ABaseHelperBot::OnDetectedChange);
	}
}

void ABaseHelperBot::LoadStats()
{
	if (UHelperBotStatsSubsystem* Subsys = GetGameInstance()->GetSubsystem<UHelperBotStatsSubsystem>())
	{
		Subsys->GetStatsRowAsync(RowName, [this](const FHelperBotStatsRow* Row)
		{
			if (!Row)
			{
				UE_LOG(LogTemp, Warning, TEXT("[HelperBot] Stats Load Failed"));
				return;
			}

			CurrentStats = *Row;

			if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = CurrentStats.WalkSpeed;
				MoveComp->JumpZVelocity = CurrentStats.JumpHeight;
			}

			CurrentHealth = CurrentStats.MaxHealth;
		});
	}
}

void ABaseHelperBot::OnDetectedChange(AActor* InteractableActor, bool bIsDetected)
{
	if (bIsDetected)
	{
		if (!InfoUIInstance && InfoUIClass)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				InfoUIInstance = CreateWidget<UHelperBotInfoWidget>(PC, InfoUIClass);
				if (InfoUIInstance)
				{
					InfoUIInstance->SetHealth(GetCurrentHealth(), CurrentStats.MaxHealth);
					InfoUIInstance->AddToViewport();
				}
			}
		}
	}
	else
	{
		if (InfoUIInstance)
		{
			InfoUIInstance->RemoveFromParent();
			InfoUIInstance = nullptr;
		}
	}
}

void ABaseHelperBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseHelperBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseHelperBot::HandleInteract(AActor* InteractableActor)
{
	if (StateUIInstance && StateUIInstance->IsInViewport())
	{
		StateUIInstance->RemoveFromParent();
		StateUIInstance = nullptr;

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
			{
				HUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
			}
		}
		return;
	}

	if (bIsWorking)
	{
		SetIsWorking(false);
		StopEyeBeamWork();
	}

	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		if (StateUIClass)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
				{
					StateUIInstance = HUD->CreateAndAddWidget<UHelperBotStateManagerWidget>(
						StateUIClass, 15, ESlateVisibility::Visible);

					if (StateUIInstance)
					{
						EHelperBotState CurrentState = static_cast<EHelperBotState>(BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
						StateUIInstance->InitializeWithController(BotAI, CurrentState);
						HUD->SetInputMode(ESurvivalInputMode::GameAndUI, StateUIInstance, true);
					}
				}
			}
		}
	}
}

void ABaseHelperBot::UpdateEyeBeamWorkTarget(AActor* TargetActor)
{
	if (!LeftEyeWorkSpline || !RightEyeWorkSpline || !TargetActor) 
	{
		return;
	}
	
	if (!LeftEyeWorkVFXComponent || !RightEyeWorkVFXComponent)
	{
		return;
	}
	
	if (!LeftEyeWorkVFXComponent->GetAsset())
	{
		return;
	}
	
	FVector LeftEyeStart = LeftEyeLight ? LeftEyeLight->GetComponentLocation() : GetActorLocation();
	FVector RightEyeStart = RightEyeLight ? RightEyeLight->GetComponentLocation() : GetActorLocation();

	FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector End = FVector(TargetLocation.X, TargetLocation.Y, LeftEyeStart.Z);

	LeftEyeWorkSpline->ClearSplinePoints(false);
	LeftEyeWorkSpline->AddSplinePoint(LeftEyeStart, ESplineCoordinateSpace::World, false);
	LeftEyeWorkSpline->AddSplinePoint(End, ESplineCoordinateSpace::World, true);
	
	LeftEyeWorkVFXComponent->SetVariableObject(FName("SplineComponent"), LeftEyeWorkSpline);
	LeftEyeWorkVFXComponent->SetVariableVec3(FName("StartLocation"), LeftEyeStart);
	LeftEyeWorkVFXComponent->SetVariableVec3(FName("EndLocation"), End);
	
	float LeftSplineLength = LeftEyeWorkSpline->GetSplineLength();
	LeftEyeWorkVFXComponent->SetVariableFloat(FName("SplineLength"), LeftSplineLength);
	LeftEyeWorkVFXComponent->SetVisibility(true);
	LeftEyeWorkVFXComponent->Activate(true);

	RightEyeWorkSpline->ClearSplinePoints(false);
	RightEyeWorkSpline->AddSplinePoint(RightEyeStart, ESplineCoordinateSpace::World, false);
	RightEyeWorkSpline->AddSplinePoint(End, ESplineCoordinateSpace::World, true);
	
	if (RightEyeWorkVFXComponent->GetAsset() != LeftEyeWorkVFXComponent->GetAsset())
	{
		RightEyeWorkVFXComponent->SetAsset(LeftEyeWorkVFXComponent->GetAsset());
	}
	
	RightEyeWorkVFXComponent->SetVariableObject(FName("SplineComponent"), RightEyeWorkSpline);
	RightEyeWorkVFXComponent->SetVariableVec3(FName("StartLocation"), RightEyeStart);
	RightEyeWorkVFXComponent->SetVariableVec3(FName("EndLocation"), End);
	
	float RightSplineLength = RightEyeWorkSpline->GetSplineLength();
	RightEyeWorkVFXComponent->SetVariableFloat(FName("SplineLength"), RightSplineLength);
	RightEyeWorkVFXComponent->SetVisibility(true);
	RightEyeWorkVFXComponent->Activate(true);
}

void ABaseHelperBot::StopEyeBeamWork()
{
	if (LeftEyeWorkVFXComponent)
	{
		LeftEyeWorkVFXComponent->Deactivate();
	}
	
	if (RightEyeWorkVFXComponent)
	{
		RightEyeWorkVFXComponent->Deactivate();
	}
}

void ABaseHelperBot::UpdateStateVisualEffects()
{
	FLinearColor TargetEyeColor = IdleEyeColor;
	UNiagaraSystem* TargetVFXAsset = nullptr;
	
	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		EHelperBotState CurrentState = static_cast<EHelperBotState>(
			BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
		
		switch (CurrentState)
		{
		case EHelperBotState::ChopWood:
		case EHelperBotState::Mining:
		case EHelperBotState::Gathering:
			TargetEyeColor = ResourceEyeColor;
			TargetVFXAsset = ResourceWorkVFX;
			break;
			
		case EHelperBotState::Defending:
			TargetEyeColor = DefendingEyeColor;
			TargetVFXAsset = DefendingWorkVFX;
			break;
			
		case EHelperBotState::Repairing:
			TargetEyeColor = RepairingEyeColor;
			TargetVFXAsset = RepairingWorkVFX;
			break;
			
		default:
			TargetEyeColor = IdleEyeColor;
			TargetVFXAsset = nullptr;
			break;
		}
	}
	
	if (LeftEyeLight)
	{
		LeftEyeLight->SetLightColor(TargetEyeColor);
	}
	if (RightEyeLight)
	{
		RightEyeLight->SetLightColor(TargetEyeColor);
	}
	
	if (LeftEyeWorkVFXComponent)
	{
		LeftEyeWorkVFXComponent->SetAsset(TargetVFXAsset);
	}
	if (RightEyeWorkVFXComponent)
	{
		RightEyeWorkVFXComponent->SetAsset(TargetVFXAsset);
	}
}