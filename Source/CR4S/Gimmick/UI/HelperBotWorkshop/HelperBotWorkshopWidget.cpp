#include "HelperBotWorkshopWidget.h"

#include "CR4S.h"
#include "HelperBotManagementWidget.h"
#include "HelperBotRepairWidget.h"
#include "HelperBotSlotWidget.h"
#include "Components/Button.h"
#include "Gimmick/Data/ItemData.h"
#include "UI/Crafting/CraftingWidget.h"

void UHelperBotWorkshopWidget::InitWidget(UPlayerInventoryComponent* PlayerInventoryComponent)
{
	InitRecipeData();

	if (IsValid(SlotContainer))
	{
		for (UWidget* Widget : SlotContainer->GetAllChildren())
		{
			UHelperBotSlotWidget* HelperBotSlot = Cast<UHelperBotSlotWidget>(Widget);
			if (IsValid(HelperBotSlot))
			{
				HelperBotSlot->InitWidget(HelperBotManagementWidget);
				HelperBotSlots.AddUnique(HelperBotSlot);
			}
		}
	}

	if (IsValid(PlayerInventoryComponent))
	{
		PlayerInventoryComponent->OnInventoryClosed.AddUniqueDynamic(this, &ThisClass::CloseOtherWidget);
	}

	if (!CR4S_VALIDATE(LogGimmickUI, IsValid(CraftingButton)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(CraftingWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotRepairWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotManagementWidget)))
	{
		return;
	}

	CraftingButton->OnClicked.AddUniqueDynamic(this, &ThisClass::UHelperBotWorkshopWidget::OpenCraftingWidget);
	CraftingWidget->InitWidget(PlayerInventoryComponent);
	CraftingWidget->CloseWidget();
	HelperBotRepairWidget->InitWidget(HelperBotManagementWidget);
	HelperBotManagementWidget->InitWidget(HelperBotRepairWidget);

	CraftingWidget->CloseWidget();
	HelperBotRepairWidget->CloseWidget();
	HelperBotManagementWidget->CloseWidget();
}

void UHelperBotWorkshopWidget::InitRecipeData()
{
	const UDataTable* RecipeDataTable = HelperBotRecipeDataHandle.DataTable;
	if (IsValid(RecipeDataTable))
	{
		if (const FItemRecipeData* RecipeData
			= RecipeDataTable->FindRow<FItemRecipeData>(HelperBotRecipeDataHandle.RowName,
			                                            TEXT("HelperBot")))
		{
			const UDataTable* ItemInfoDataTable = RecipeData->ItemInfoDataHandle.DataTable;
			if (IsValid(ItemInfoDataTable))
			{
				if (const FItemInfoData* ItemInfoData
					= ItemInfoDataTable->FindRow<FItemInfoData>(RecipeData->ItemInfoDataHandle.RowName,
					                                            TEXT("HelperBot")))
				{
					HelperBotRecipeData = FRecipeSelectData(*RecipeData,
					                                        ItemInfoData->Icon,
					                                        ItemInfoData->Name,
					                                        ItemInfoData->Description);
				}
			}
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UHelperBotWorkshopWidget::CloseOtherWidget()
{
	if (!CR4S_VALIDATE(LogGimmickUI, IsValid(CraftingWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotRepairWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotManagementWidget)))
	{
		return;
	}

	HelperBotRepairWidget->CloseWidget();
	HelperBotManagementWidget->CloseWidget();
	CraftingWidget->CloseWidget();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UHelperBotWorkshopWidget::OpenCraftingWidget()
{
	if (!CR4S_VALIDATE(LogGimmickUI, IsValid(CraftingWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotRepairWidget)) ||
		!CR4S_VALIDATE(LogGimmickUI, IsValid(HelperBotManagementWidget)))
	{
		return;
	}

	HelperBotRepairWidget->CloseWidget();
	HelperBotManagementWidget->CloseWidget();
	
	CraftingWidget->OpenWidget(HelperBotRecipeData);
}

void UHelperBotWorkshopWidget::CloseCraftingWidget() const
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->CloseWidget();
	}
}
