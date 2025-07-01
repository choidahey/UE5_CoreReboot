#include "CraftingWidget.h"

#include "CR4S.h"
#include "ButtonWidget/RecipeSelectButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Game/System/AudioManager.h"
#include "Gimmick/Data/ItemData.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/InventoryItem/RobotPartsInventoryItem.h"
#include "Inventory/UI/ItemTooltipWidget.h"
#include "Utility/Cr4sGameplayTags.h"

UCraftingWidget::UCraftingWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	  bCanCraft(false)
{
}

void UCraftingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ItemIcon))
	{
		ItemIcon->ToolTipWidgetDelegate.BindDynamic(this, &ThisClass::ShowToolTip);
	}
}

void UCraftingWidget::InitWidget(UPlayerInventoryComponent* NewPlayerInventoryComponent)
{
	PlayerInventoryComponent = NewPlayerInventoryComponent;

	if (IsValid(IngredientsContainer))
	{
		for (UWidget* Widget : IngredientsContainer->GetAllChildren())
		{
			UIngredientsWidget* IngredientWidget = Cast<UIngredientsWidget>(Widget);
			if (IsValid(IngredientWidget))
			{
				IngredientWidgets.AddUnique(IngredientWidget);
				IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (IsValid(CraftButton))
	{
		CraftButton->OnClicked.AddUniqueDynamic(this, &ThisClass::CraftItem);
		CraftButton->SetIsEnabled(false);
	}
}

void UCraftingWidget::UpdateResultItem() const
{
	const UDataTable* DataTable = ItemRecipeData.ItemInfoDataHandle.DataTable;
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(DataTable)))
	{
		return;
	}

	const FName RowName = ItemRecipeData.ItemInfoDataHandle.RowName;
	if (const FItemInfoData* ItemData
		= DataTable->FindRow<FItemInfoData>(RowName,TEXT("Item")))
	{
		const FGameplayTagContainer& ItemTags = ItemData->ItemTags;			
		if (ItemTags.HasTag(ItemTags::RobotParts) || ItemTags.HasTag(ItemTags::Weapon))
		{
			URobotPartsInventoryItem* RobotPartsItem = Cast<URobotPartsInventoryItem>(ResultItem);
			if (IsValid(RobotPartsItem))
			{
				RobotPartsItem->SetItemInfoData(*ItemData);
				RobotPartsItem->InitTextFormat();
			}
		}
	}
}

void UCraftingWidget::OpenWidget(const FRecipeSelectData& RecipeSelectData)
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemIcon)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemName)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(ItemDescription)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(CraftButton)) ||
		!CR4S_VALIDATE(LogCraftingUI, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	ItemRecipeData = RecipeSelectData.RecipeData;

	UpdateResultItem();

	ItemIcon->SetBrushFromTexture(RecipeSelectData.Icon, true);
	ItemName->SetText(RecipeSelectData.Name);
	ItemDescription->SetText(RecipeSelectData.Description);

	bCanCraft = true;
	CurrentIngredients.Empty();

	const TArray<FRecipeIngredient>& Ingredients = RecipeSelectData.RecipeData.Ingredients;
	for (int32 Index = 0; Index < IngredientWidgets.Num(); Index++)
	{
		UIngredientsWidget* IngredientWidget = IngredientWidgets[Index];
		if (!IsValid(IngredientWidget))
		{
			continue;
		}

		if (Ingredients.IsValidIndex(Index))
		{
			const FRecipeIngredient& Ingredient = Ingredients[Index];
			const UDataTable* DataTable = Ingredient.ItemInfoDataHandle.DataTable;
			if (!CR4S_VALIDATE(LogCraftingUI, IsValid(DataTable)))
			{
				bCanCraft = false;
				continue;
			}

			const FName RowName = Ingredient.ItemInfoDataHandle.RowName;
			if (const FItemInfoData* ItemData
				= DataTable->FindRow<FItemInfoData>(RowName,TEXT("Item")))
			{
				const int32 CurrentCount = PlayerInventoryComponent->GetItemCountByRowName(RowName);
				const int32 RequiredCount = Ingredient.Count;

				if (bCanCraft && CurrentCount < RequiredCount)
				{
					bCanCraft = false;
				}

				FIngredientData IngredientData = FIngredientData(ItemData->Icon,
				                                                 ItemData->Name,
				                                                 CurrentCount,
				                                                 RequiredCount);
				CurrentIngredients.Add(IngredientData);

				IngredientWidget->InitWidget(IngredientData);
				IngredientWidget->SetVisibility(ESlateVisibility::Visible);
			}
		}
		else
		{
			IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	CraftButton->SetIsEnabled(bCanCraft);

	SetVisibility(ESlateVisibility::Visible);
}

void UCraftingWidget::CloseWidget()
{
	for (UIngredientsWidget* IngredientWidget : IngredientWidgets)
	{
		IngredientWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UCraftingWidget::CreateResultItem(const FGameplayTagContainer& ItemTags)
{
	ResultItem = UItemGimmickSubsystem::CreateInventoryItem(this, ItemTags);
}

void UCraftingWidget::CraftItem()
{
	if (!CR4S_VALIDATE(LogCraftingUI, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	PlayCraftingSound();

	for (const auto& [DataTableHandle, Count] : ItemRecipeData.Ingredients)
	{
		PlayerInventoryComponent->RemoveItemByRowName(DataTableHandle.RowName, Count);
	}

	PlayerInventoryComponent->AddItem(ItemRecipeData.ItemInfoDataHandle.RowName, ItemRecipeData.ResultCount);

	if (OnCraftingComplete.IsBound())
	{
		OnCraftingComplete.Broadcast();
	}

	for (int32 Index = 0; Index < CurrentIngredients.Num(); Index++)
	{
		if (!IngredientWidgets.IsValidIndex(Index))
		{
			return;
		}

		const UIngredientsWidget* IngredientWidget = IngredientWidgets[Index];
		if (IsValid(IngredientWidget))
		{
			FIngredientData& IngredientData = CurrentIngredients[Index];

			IngredientData.CurrentCount -= IngredientData.RequiredCount;

			IngredientWidget->InitWidget(IngredientData);

			if (bCanCraft && IngredientData.CurrentCount < IngredientData.RequiredCount)
			{
				bCanCraft = false;
			}
		}

		CraftButton->SetIsEnabled(bCanCraft);
	}
}

void UCraftingWidget::PlayCraftingSound() const
{
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(CraftingSound)))
	{
		return;
	}

	const UGameInstance* GameInstance = GetGameInstance();
	if (!CR4S_VALIDATE(LogInventoryUI, IsValid(GameInstance)))
	{
		return;
	}

	UAudioManager* AudioManager = GameInstance->GetSubsystem<UAudioManager>();
	if (IsValid(AudioManager))
	{
		AudioManager->PlayUISound(CraftingSound);
	}
}

UWidget* UCraftingWidget::ShowToolTip()
{
	if (!CR4S_VALIDATE(LogInventoryUI, ItemTooltipWidgetClass))
	{
		return nullptr;
	}

	if (!IsValid(ItemTooltipWidget))
	{
		ItemTooltipWidget = CreateWidget<UItemTooltipWidget>(this, ItemTooltipWidgetClass);
	}

	ItemTooltipWidget->InitWidget(ResultItem);

	return ItemTooltipWidget;
}
