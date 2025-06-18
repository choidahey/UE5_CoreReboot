#include "RobotInventoryWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Gimmick/Data/RecipeCategoryData.h"
#include "UI/Crafting/CraftingWidget.h"
#include "UI/Crafting/RecipeSelectWidget.h"

void URobotInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bCanSort = false;
	bCanDrag = false;
	bCanDrop = false;
	bCanRemoveItem = false;
	bCanMoveItem = false;

	RobotWorkshopMode = ERobotWorkshopMode::Assembly;
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

void URobotInventoryWidget::ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent)
{
	Super::ConnectInventoryComponent(NewInventoryComponent);

	ChangeModeCreate();
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
				if (RecipeTag.MatchesTag(Data->RecipeTag))
				{
					RecipeItems.AddUnique(Data);
				}
			}
		}

		RecipeSelectWidget->OpenWidget(Icon, Name, RecipeItems);
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
	const FString ModeString = UEnum::GetValueAsString(NewRobotWorkshopMode);
	if (RobotWorkshopMode == NewRobotWorkshopMode)
	{
		return;
	}

	RobotWorkshopMode = NewRobotWorkshopMode;

	CloseCraftingWidget();
	CloseRecipeSelectWidget();

	const bool bIsCreateMode = RobotWorkshopMode == ERobotWorkshopMode::Create;
	
	if (IsValid(InventoryTitleTextBlock))
	{
		InventoryTitleTextBlock->SetText(bIsCreateMode
			                                 ? CreateModeText
			                                 : AssemblyModeText);
	}

	if (OnRobotWorkshopModeChange.IsBound())
	{
		OnRobotWorkshopModeChange.Broadcast(RobotWorkshopMode);
	}
	
	bCanDrag = !bIsCreateMode;
	bCanDrop = !bIsCreateMode;
}
