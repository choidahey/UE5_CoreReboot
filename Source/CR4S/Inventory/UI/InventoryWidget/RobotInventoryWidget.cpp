#include "RobotInventoryWidget.h"

#include "Character/Characters/ModularRobot.h"
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
	if (IsValid(NewInventoryComponent))
	{
		ModularRobot = Cast<AModularRobot>(NewInventoryComponent->GetOwner());

		if (IsValid(ModularRobot))
		{
			RobotStatusComponent = ModularRobot->GetStatusComponent();
		}

		BindDelegate();
	}

	Super::ConnectInventoryComponent(NewInventoryComponent);

	ChangeModeCreate();
}

void URobotInventoryWidget::UnBoundWidgetDelegate()
{
	Super::UnBoundWidgetDelegate();

	if (IsValid(RobotStatusComponent))
	{
		if (RobotStatusComponent->OnWeightChanged.IsAlreadyBound(this, &ThisClass::UpdateCurrentWeightText))
		{
			RobotStatusComponent->OnWeightChanged.RemoveDynamic(this, &ThisClass::UpdateCurrentWeightText);
		}

		if (RobotStatusComponent->OnWeightChanged.IsAlreadyBound(this, &ThisClass::UpdateMaxWeightText))
		{
			RobotStatusComponent->OnMaxWeightChanged.RemoveDynamic(this, &ThisClass::UpdateMaxWeightText);
		}
	}

	ModularRobot = nullptr;
	RobotStatusComponent = nullptr;
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
				if (Data->RecipeTag.MatchesTag(RecipeTag))
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

	UpdateButtonColor();

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

void URobotInventoryWidget::BindDelegate()
{
	if (IsValid(RobotStatusComponent))
	{
		RobotStatusComponent->OnWeightChanged.AddUniqueDynamic(this, &ThisClass::UpdateCurrentWeightText);
		RobotStatusComponent->OnMaxWeightChanged.AddUniqueDynamic(this, &ThisClass::UpdateMaxWeightText);
		RobotStatusComponent->OnArmLoadChanged.AddUniqueDynamic(this, &ThisClass::UpdateCurrentArmWeightText);
		RobotStatusComponent->OnMaxArmLoadChanged.AddUniqueDynamic(this, &ThisClass::UpdateMaxArmWeightText);

		UpdateCurrentWeightText(RobotStatusComponent->GetCurrentWeight());
		UpdateMaxWeightText(RobotStatusComponent->GetMaxWeight());
		UpdateCurrentArmWeightText(RobotStatusComponent->GetCurrentArmWeight());
		UpdateMaxArmWeightText(RobotStatusComponent->GetMaxArmWeight());
	}
}

void URobotInventoryWidget::UpdateCurrentWeightText(const float NewValue)
{
	if (IsValid(CurrentWeightText))
	{
		CurrentWeight = NewValue;

		CurrentWeightText->SetText(FText::AsNumber(CurrentWeight));

		UpdateCurrentWeightColor(false);
	}
}

void URobotInventoryWidget::UpdateMaxWeightText(const float NewValue)
{
	if (IsValid(MaxWeightText))
	{
		MaxWeight = NewValue;
		MaxWeightText->SetText(FText::AsNumber(MaxWeight));

		UpdateCurrentWeightColor(false);
	}
}

void URobotInventoryWidget::UpdateCurrentArmWeightText(const float NewValue)
{
	if (IsValid(CurrentArmWeightText))
	{
		CurrentArmWeight = NewValue;

		CurrentArmWeightText->SetText(FText::AsNumber(CurrentArmWeight));

		UpdateCurrentWeightColor(true);
	}
}

void URobotInventoryWidget::UpdateMaxArmWeightText(const float NewValue)
{
	if (IsValid(MaxArmWeightText))
	{
		MaxArmWeight = NewValue;
		MaxArmWeightText->SetText(FText::AsNumber(MaxArmWeight));

		UpdateCurrentWeightColor(false);
	}
}

void URobotInventoryWidget::UpdateCurrentWeightColor(const bool bIsArmWeight) const
{
	if (bIsArmWeight)
	{
		if (IsValid(CurrentArmWeightText))
		{
			CurrentArmWeightText->SetColorAndOpacity(CurrentArmWeight <= MaxArmWeight ? DefaultColor : OverColor);
		}
	}
	else
	{
		if (IsValid(CurrentWeightText))
		{
			CurrentWeightText->SetColorAndOpacity(CurrentWeight <= MaxWeight ? DefaultColor : OverColor);
		}		
	}
}

void URobotInventoryWidget::UpdateButtonColor() const
{
	if (!IsValid(CreateModeButton) || !IsValid(AssemblyModeButton))
	{
		return;
	}
	
	const bool bIsCreateMode = RobotWorkshopMode == ERobotWorkshopMode::Create;

	CreateModeButton->SetBackgroundColor(bIsCreateMode ? CurrentModeButtonColor : FLinearColor::White);
	AssemblyModeButton->SetBackgroundColor(bIsCreateMode ? FLinearColor::White : CurrentModeButtonColor);
}
