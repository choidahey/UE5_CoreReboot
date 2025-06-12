#include "ItemTooltipWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/ItemData.h"

void UItemTooltipWidget::InitWidget(const FItemInfoData& ItemInfo) const
{
	if (IsValid(ItemIcon))
	{
		ItemIcon->SetBrushFromTexture(ItemInfo.Icon, true);
	}

	if (IsValid(ItemName))
	{
		ItemName->SetText(ItemInfo.Name);
	}

	if (IsValid(ItemDescription))
	{
		ItemDescription->SetText(ItemInfo.Description);
	}
}
