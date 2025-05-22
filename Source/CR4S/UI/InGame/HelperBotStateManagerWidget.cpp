#include "HelperBotStateManagerWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../FriendlyAI/Controller/HelperBotAIController.h"

void UHelperBotStateManagerWidget::InitializeWithController(AHelperBotAIController* InController)
{
	OwnerAIController = InController;
}

void UHelperBotStateManagerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button1) Button1->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton1Clicked);
	if (Button2) Button2->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton2Clicked);
	if (Button3) Button3->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton3Clicked);
	if (Button4) Button4->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton4Clicked);
	if (Button5) Button5->OnClicked.AddDynamic(this, &UHelperBotStateManagerWidget::OnButton5Clicked);
}

void UHelperBotStateManagerWidget::OnButton1Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Idle);
}

void UHelperBotStateManagerWidget::OnButton2Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::Following);
}

void UHelperBotStateManagerWidget::OnButton3Clicked()
{
	if (OwnerAIController) OwnerAIController->SetBotState(EHelperBotState::ChopWood);
}

void UHelperBotStateManagerWidget::OnButton4Clicked()
{
	RemoveFromParent();
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
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
}
