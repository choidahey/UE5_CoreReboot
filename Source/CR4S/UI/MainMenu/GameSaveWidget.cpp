#include "UI/MainMenu/GameSaveWidget.h"
#include "UI/Common/ButtonWidget.h"


void UGameSaveWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BackButton)
	{
		BackButton->OnClicked().AddDynamic(this, &UGameSaveWidget::HandleCloseWindow);
	}
}

void UGameSaveWidget::HandleOpenWindow()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UGameSaveWidget::HandleCloseWindow()
{
	SetVisibility(ESlateVisibility::Hidden);
}