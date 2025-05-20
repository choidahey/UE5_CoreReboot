#include "BaseHelperBot.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Controller/HelperBotAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperBotStatsSubsystem.h"
#include "../Gimmick/Components/InteractableComponent.h"

/**
 * 
 */
ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));
	InteractableComp->SetInteractionText(FText::FromString("MySon"));
	InteractableComp->OnTryInteract.BindUObject(this, &ABaseHelperBot::HandleInteract);
}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();

	LoadStats();

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetAvoidanceEnabled(true);
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

void ABaseHelperBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseHelperBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseHelperBot::HandleInteract()
{
	if (StateUIInstance && StateUIInstance->IsInViewport())
	{
		StateUIInstance->RemoveFromParent();
		StateUIInstance = nullptr;
	
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->bShowMouseCursor = false;
		}
		return;
	}
	
	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		if (StateUIClass)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				StateUIInstance = CreateWidget<UHelperBotStateManagerWidget>(PC, StateUIClass);
				if (StateUIInstance)
				{
					StateUIInstance->InitializeWithController(BotAI);
					StateUIInstance->AddToViewport();
					PC->SetInputMode(FInputModeGameAndUI());
					PC->bShowMouseCursor = true;
				}
			}
		}
	}
}