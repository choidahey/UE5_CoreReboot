#include "RobotInventoryWidget.h"

#include "CR4S.h"
#include "Components/Button.h"
#include "Gimmick/Data/RecipeCategoryData.h"
#include "UI/Crafting/CraftingWidget.h"
#include "UI/Crafting/RecipeSelectWidget.h"

void URobotInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bCanSort = false;
	bCanDrag = true;
	bCanDrop = true;
	bCanRemoveItem = true;
	bCanMoveItem = true;
}

void URobotInventoryWidget::InitWidget(ASurvivalHUD* SurvivalHUD, const bool bNewCanSort)
{
	Super::InitWidget(SurvivalHUD, bNewCanSort);

	if (IsValid(CreateModeButton))
	{
		CreateModeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::ChangeModeCreate);
		AssemblyModeButton->OnClicked.AddUniqueDynamic(this, &ThisClass::ChangeModeAssembly);
	}

	CloseCraftingWidget();

	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->InitWidget(CraftingWidget);

		CloseRecipeSelectWidget();
	}

	if (IsValid(CraftingWidget) && IsValid(InventoryContainerWidget))
	{
		CraftingWidget->InitWidget(InventoryContainerWidget->GetPlayerInventoryComponent());
	}
}

void URobotInventoryWidget::OpenRecipeSelectWidget(UTexture2D* Icon, const FText& Name,
                                                   const FGameplayTag& RecipeTag) const
{
	if (IsValid(RecipeSelectWidget) && IsValid(ItemRecipeData))
	{
		TArray<const FItemRecipeData*> RecipeItems;
		for (auto& Pair : ItemRecipeData->GetRowMap())
		{
			if (const FItemRecipeData* Data = reinterpret_cast<FItemRecipeData*>(Pair.Value))
			{
				if (Data->RecipeTag == RecipeTag)
				{
					RecipeItems.AddUnique(Data);
				}
			}
		}

		if (RecipeItems.Num() > 0)
		{
			RecipeSelectWidget->OpenWidget(Icon, Name, RecipeItems);
		}
		else
		{
			RecipeSelectWidget->CloseWidget();
		}
	}

	CloseCraftingWidget();
}

void URobotInventoryWidget::CloseRecipeSelectWidget() const
{
	if (IsValid(RecipeSelectWidget))
	{
		RecipeSelectWidget->CloseWidget();
	}
}

void URobotInventoryWidget::CloseCraftingWidget() const
{
	if (IsValid(CraftingWidget))
	{
		CraftingWidget->CloseWidget();
	}
}

void URobotInventoryWidget::ChangeModeCreate()
{
	ChangeMode(ERobotWorkshopMode::Create);
}

void URobotInventoryWidget::ChangeModeAssembly()
{
	ChangeMode(ERobotWorkshopMode::Assembly);
}

void URobotInventoryWidget::ChangeMode(const ERobotWorkshopMode NewRobotWorkshopMode)
{
	if (RobotWorkshopMode == NewRobotWorkshopMode)
	{
		const FString ModeString = UEnum::GetValueAsString(NewRobotWorkshopMode);
		CR4S_Log(LogInventoryUI, Warning, TEXT("Already %s Mode"), *ModeString);
		return;
	}

	RobotWorkshopMode = NewRobotWorkshopMode;

	CloseCraftingWidget();
	CloseRecipeSelectWidget();
}
