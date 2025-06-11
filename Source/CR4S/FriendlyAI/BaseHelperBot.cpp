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
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"


ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->SetInteractionText(FText::FromString("MySon"));

	ChopSpline = CreateDefaultSubobject<USplineComponent>(TEXT("ChopSpline"));
	ChopSpline->SetupAttachment(RootComponent);

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));
}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();

	LoadStats();

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
						StateUIInstance->InitializeWithController(BotAI);
						HUD->SetInputMode(ESurvivalInputMode::GameAndUI, StateUIInstance, true);
					}
				}
			}
		}
	}
}

void ABaseHelperBot::UpdateChopSplineTarget(AActor* TargetActor)
{
	if (!ChopSpline || !TargetActor) return;
	
	const FVector Start = GetMesh()
		? GetMesh()->GetSocketLocation(TEXT("test"))
		: GetActorLocation();

	const FVector End = TargetActor->GetActorLocation();

	ChopSpline->ClearSplinePoints(false);
	ChopSpline->AddSplinePoint(Start, ESplineCoordinateSpace::World, false);
	ChopSpline->AddSplinePoint(End, ESplineCoordinateSpace::World, true);
	
	if (ChopVFXSystem && !ActiveChopVFX)
	{
		FVector SpawnLocation = Start;
		FRotator SpawnRotation = FRotator::ZeroRotator;
		UNiagaraComponent* Spawned = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ChopVFXSystem,
			SpawnLocation,
			SpawnRotation
		);
		if (Spawned)
		{
			// Spawned->SetNiagaraVariableVec3(FName("Start"), Start);
			// Spawned->SetNiagaraVariableVec3(FName("End"), End);
			ActiveChopVFX = Spawned;
		}
	}
}
