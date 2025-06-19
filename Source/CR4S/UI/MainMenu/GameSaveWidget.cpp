#include "UI/MainMenu/GameSaveWidget.h"
#include "UI/Common/ButtonWidget.h"
#include "UI/Common/ConfirmWidget.h"
#include "UI/MainMenu/SaveSlotWidget.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "Game/GameMode/C4MenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"

void UGameSaveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BackButton)
	{
		BackButton->OnClicked().AddDynamic(this, &UGameSaveWidget::HandleCloseWindow);
	}

	if (SaveSlot1)
		SaveSlot1->OnClicked.AddDynamic(this, &UGameSaveWidget::HandleEmptySlot0);
	if (SaveSlot2)
		SaveSlot2->OnClicked.AddDynamic(this, &UGameSaveWidget::HandleEmptySlot1);
	if (SaveSlot3)
		SaveSlot3->OnClicked.AddDynamic(this, &UGameSaveWidget::HandleEmptySlot2);
	if (SaveSlot4)
		SaveSlot4->OnClicked.AddDynamic(this, &UGameSaveWidget::HandleEmptySlot3);

	SaveSlotButtons = { SaveSlot1, SaveSlot2, SaveSlot3, SaveSlot4 };

	SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>();
}

void UGameSaveWidget::HandleOpenWindow()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UGameSaveWidget::HandleCloseWindow()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UGameSaveWidget::HandleEmptySlotSelected(int32 SlotIndex)
{
	SelectedSlotIndex = SlotIndex;

	ConfirmWidget = CreateWidget<UConfirmWidget>(GetWorld(), ConfirmWidgetClass);
	if (ConfirmWidget)
	{
		ConfirmWidget->AddToViewport();
		ConfirmWidget->Confirm(FText::FromString(FString::Printf(TEXT("Start New Game On slot %d?"), SlotIndex)));
		ConfirmWidget->OnYes.BindDynamic(this, &UGameSaveWidget::HandleStartNewGame);
		ConfirmWidget->OnNo.BindDynamic(this, &UGameSaveWidget::HandleCloseConfirmWidget);
	}
}

void UGameSaveWidget::HandleStartNewGame()
{
	HandleCloseConfirmWidget();

	if (!SaveSlotButtons.IsValidIndex(SelectedSlotIndex)) return;

	UButton* TargetButton = SaveSlotButtons[SelectedSlotIndex];
	if (!TargetButton) return;

	UBorder* ParentBorder = Cast<UBorder>(TargetButton->GetParent());

	USaveSlotWidget* NewSlotWidget = CreateWidget<USaveSlotWidget>(GetWorld(), SaveSlotWidgetClass);
	if (!NewSlotWidget) return;
	
	NewSlotWidget->SetVisibility(ESlateVisibility::Collapsed);
	ParentBorder->SetContent(NewSlotWidget);
	NewSlotWidget->PlayCreateAnimation();
	NewSlotWidget->SetVisibility(ESlateVisibility::Visible);

	SaveSlots.Add(NewSlotWidget);

	SaveGameManager->CreateSlot(FString::FromInt(SelectedSlotIndex));
	NewSlotWidget->UpdateSlotInfo(SaveGameManager->GetSaveMetaDataByIndex(SelectedSlotIndex));



	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			AC4MenuGameMode* GM = Cast<AC4MenuGameMode>(UGameplayStatics::GetGameMode(this));
			if (GM)
			{
				GM->OpenSurvivalLevel(SelectedSlotIndex);
			}
		}), 0.3f, false);
}


void UGameSaveWidget::HandleCloseConfirmWidget()
{
	if (ConfirmWidget)
	{
		ConfirmWidget->RemoveFromParent();
		ConfirmWidget = nullptr;
	}
}
