#include "HelperBotStateManagerWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../FriendlyAI/Controller/HelperBotAIController.h"
#include "../../FriendlyAI/BaseHelperBot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Inventory/OpenWidgetType.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Character/Characters/PlayerCharacter.h"

void UHelperBotStateManagerWidget::InitializeWithController(AHelperBotAIController* InController)
{
	OwnerAIController = InController;
	if (InController)
	{
		HelperBot = Cast<ABaseHelperBot>(InController->GetPawn());
	}
}

void UHelperBotStateManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (SetIdleButton) SetIdleButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetIdle);
	if (SetFollowingButton) SetFollowingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetFollowing);
	if (SetChopWoodButton) SetChopWoodButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetChopWood);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::CloseStateWidget);
	if (OpenInventoryButton) OpenInventoryButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OpenInventory);
	if (SetMiningButton) SetMiningButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetMining);
	if (SetRepairingButton) SetRepairingButton->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::SetRepairing);

	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}

void UHelperBotStateManagerWidget::SetIdle()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Idle);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetFollowing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Following);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetChopWood()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::ChopWood);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::CloseStateWidget()
{
	CloseWidgetAndResetInput();
}


void UHelperBotStateManagerWidget::OpenInventory()
{
	if (!HelperBot)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC)
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		return;
	}

	UPlayerInventoryComponent* PlayerInvComp = PlayerCharacter->FindComponentByClass<UPlayerInventoryComponent>();
	UBaseInventoryComponent* AIInvComp     = HelperBot->FindComponentByClass<UBaseInventoryComponent>();
	if (!PlayerInvComp || !AIInvComp)
	{
		return;
	}

	PlayerInvComp->OpenOtherInventoryWidget(EOpenWidgetType::ItemPouch, AIInvComp);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetMining()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Mining);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::SetRepairing()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Repairing);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::CloseWidgetAndResetInput()
{
	if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()))
	{
		HUD->ToggleWidget(this);
		HUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}
}
