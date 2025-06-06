#include "IngredientWidget.h"

#include "CR4S.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UIngredientWidget::InitWidget(const FIngredientData& IngredientData) const
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(IngredientIcon)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(IngredientName)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(RequiredIngredientCount)))
	{
		return;
	}

	IngredientIcon->SetBrushFromTexture(IngredientData.Icon, true);
	IngredientName->SetText(IngredientData.Name);

	const FString CountString = FString::Printf(TEXT("%d/%d"),
	                                            IngredientData.CurrentCount,
	                                            IngredientData.RequiredCount);

	RequiredIngredientCount->SetText(FText::FromString(CountString));

	UpdateProgressBar(IngredientData.CurrentCount, IngredientData.RequiredCount);
}

void UIngredientWidget::UpdateProgressBar(const int32 CurrentCount, const int32 RequiredCount) const
{
	if (!IsValid(ResourceProgressBar))
	{
		return;
	}

	const float Divider = FMath::Max(RequiredCount, KINDA_SMALL_NUMBER);
	const float Percent = FMath::Clamp(CurrentCount / Divider, 0.f, 1.f);
	
	ResourceProgressBar->SetPercent(Percent);
	
	const FLinearColor CurColor = FLinearColor::LerpUsingHSV(ColorAtZero, ColorAtOne, Percent);
	ResourceProgressBar->SetFillColorAndOpacity(CurColor);
}
