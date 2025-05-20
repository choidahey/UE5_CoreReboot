#include "BaseHelperBot.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Controller/HelperBotAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Gimmick/Components/InteractableComponent.h"



ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();

	if (InteractableComp)
	{
		InteractableComp->SetInteractionText(FText::FromString("MySon"));
		InteractableComp->OnTryInteract.BindUObject(this, &ABaseHelperBot::HandleInteract);
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetAvoidanceEnabled(true);
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