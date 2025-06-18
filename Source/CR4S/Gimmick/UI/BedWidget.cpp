#include "BedWidget.h"

#include "CR4S.h"
#include "Components/Button.h"
#include "UI/InGame/SurvivalHUD.h"

void UBedWidget::InitWidget(APlayerController* PlayerController, const bool bCanSleep)
{
	if (IsValid(PlayerController))
	{
		SurvivalHUD = Cast<ASurvivalHUD>(PlayerController->GetHUD());
		if (IsValid(SurvivalHUD))
		{
			SurvivalHUD->SetInputMode(ESurvivalInputMode::UIOnly, this, true);
		}
	}
	
	if (IsValid(SleepButton))
	{
		if (bCanSleep)
		{
			SleepButton->SetIsEnabled(true);
			SleepButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleSleep);
		}
		else
		{
			SleepButton->SetIsEnabled(false);
		}
	}

	if (IsValid(CancelButton))
	{
		CancelButton->OnClicked.AddUniqueDynamic(this, &ThisClass::RemoveWidget);
	}
}

void UBedWidget::HandleSleep()
{
	CR4S_Log(LogInventoryUI, Warning, TEXT("Sleep!"));

	RemoveWidget();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UBedWidget::RemoveWidget()
{
	if (IsValid(SurvivalHUD))
	{
		SurvivalHUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
	}

	RemoveFromParent();
}
