#include "HelperBotStateManagerWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../FriendlyAI/Controller/HelperBotAIController.h"
#include "../../FriendlyAI/BaseHelperBot.h"
#include "Character/Characters/PlayerCharacter.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Inventory/InventoryType.h"
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

	if (Button1) Button1->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton1Clicked);
	if (Button2) Button2->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton2Clicked);
	if (Button3) Button3->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton3Clicked);
	if (Button4) Button4->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton4Clicked);
	if (Button5) Button5->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton5Clicked);
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

void UHelperBotStateManagerWidget::OnButton1Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Idle);
	RemoveFromParent();
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::OnButton2Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Following);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::OnButton3Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::ChopWood);
	//CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::OnButton4Clicked()
{
	CloseWidgetAndResetInput();
}


void UHelperBotStateManagerWidget::OpenInventory()
{
	//RemoveFromParent();

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

	PlayerInvComp->OpenOtherInventoryWidget(EInventoryType::ItemPouch, AIInvComp);
	CloseWidgetAndResetInput();
}

void UHelperBotStateManagerWidget::OnButton5Clicked()
{
	if (OwnerAIController)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			OwnerAIController->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
			OwnerAIController->SetBotState(EHelperBotState::Fleeing);
		}
	}
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
