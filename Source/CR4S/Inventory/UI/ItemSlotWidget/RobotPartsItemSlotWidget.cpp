#include "RobotPartsItemSlotWidget.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Components/RobotWeaponComponent.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "Inventory/UI/InventoryWidget/RobotInventoryWidget.h"
#include "Utility/Cr4sGameplayTags.h"

void URobotPartsItemSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(BackgroundIcon))
	{
		BackgroundIcon->SetBrush(BackgroundIconBrush);
	}
}

void URobotPartsItemSlotWidget::InitSlotWidgetData(UBaseInventoryWidget* NewInventoryWidget,
                                                   UBaseInventoryItem* NewItem)
{
	Super::InitSlotWidgetData(NewInventoryWidget, NewItem);

	RobotInventoryWidget = Cast<URobotInventoryWidget>(NewInventoryWidget);

	if (IsValid(RobotInventoryWidget))
	{
		RobotInventoryWidget->OnRobotWorkshopModeChange.AddUniqueDynamic(this, &ThisClass::UpdateRobotWorkshopMode);
		ModularRobot = RobotInventoryWidget->GetModularRobot();

		if (IsValid(ModularRobot))
		{
			RobotWeaponComponent = ModularRobot->GetWeaponComponent();
		}
	}

	if (IsValid(SelectButton))
	{
		if (bCanCreateParts)
		{
			SelectButton->OnClicked.AddUniqueDynamic(this, &ThisClass::OpenRecipeSelectWidget);
		}
		SelectButton->SetIsEnabled(false);
	}
}

bool URobotPartsItemSlotWidget::IsItemAllowedByFilter(UBaseInventoryItem* Item) const
{
	if (Super::IsItemAllowedByFilter(Item))
	{
		if (IsValid(Item))
		{
			const FGameplayTagContainer& RobotPartsTag = Item->GetItemTags();
			if (RobotPartsTag.HasTag(PartsTypeTag))
			{
				return EquippedBodyParts(RobotPartsTag);
			}
		}
	}

	return false;
}

void URobotPartsItemSlotWidget::SetItem(UBaseInventoryItem* InItem)
{
	Super::SetItem(InItem);

	if (IsValid(CurrentItem))
	{
		EquipParts(CurrentItem);
	}
	else
	{
		UnEquipParts();
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URobotPartsItemSlotWidget::OpenRecipeSelectWidget()
{
	if (IsValid(RobotInventoryWidget))
	{
		UTexture2D* Icon = Cast<UTexture2D>(BackgroundIcon->GetBrush().GetResourceObject());
		RobotInventoryWidget->OpenRecipeSelectWidget(Icon, PartsTypeName, PartsTypeTag);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void URobotPartsItemSlotWidget::UpdateRobotWorkshopMode(const ERobotWorkshopMode RobotWorkshopMode)
{
	const bool bIsCreateMode = RobotWorkshopMode == ERobotWorkshopMode::Create;

	if (!bCanCreateParts)
	{
		SetVisibility(bIsCreateMode ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
	else if (IsValid(SelectButton))
	{
		SelectButton->SetIsEnabled(bIsCreateMode);
	}

	bCanDrag = !bIsCreateMode;
	bCanDrop = !bIsCreateMode;
}

void URobotPartsItemSlotWidget::EquipParts(const UBaseInventoryItem* Item) const
{
	if (IsValid(ModularRobot))
	{
		const FGameplayTag ItemTag = Item->GetItemTags().First();

		if (PartsTypeTag.MatchesTag(RobotParts::Body))
		{
			ModularRobot->EquipBodyParts(ItemTag);
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Arm))
		{
			ModularRobot->EquipArmParts(ItemTag);
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Leg))
		{
			ModularRobot->EquipLegParts(ItemTag);
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Core))
		{
			ModularRobot->EquipCoreParts(ItemTag);
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Booster))
		{
			ModularRobot->EquipBoosterParts(ItemTag);
		}
		else if (IsValid(RobotWeaponComponent) && WeaponSlotIndex != -1)
		{
			RobotWeaponComponent->EquipWeaponByTag(ItemTag, WeaponSlotIndex);
		}
	}
}

void URobotPartsItemSlotWidget::UnEquipParts() const
{
	if (IsValid(ModularRobot))
	{
		if (PartsTypeTag.MatchesTag(RobotParts::Body))
		{
			ModularRobot->UnequipBodyParts();
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Arm))
		{
			ModularRobot->UnequipArmParts();
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Leg))
		{
			ModularRobot->UnequipLegParts();
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Core))
		{
			ModularRobot->UnequipCoreParts();
		}
		else if (PartsTypeTag.MatchesTag(RobotParts::Booster))
		{
			ModularRobot->UnequipBoosterParts();
		}
		else if (IsValid(RobotWeaponComponent) && WeaponSlotIndex != -1)
		{
			RobotWeaponComponent->UnequipWeapon(WeaponSlotIndex);
		}
	}
}

bool URobotPartsItemSlotWidget::EquippedBodyParts(const FGameplayTagContainer& ItemTags) const
{
	if (ItemTags.HasTag(RobotParts::Body))
	{
		return true;
	}

	if (IsValid(ModularRobot) &&
		CR4S_VALIDATE(LogGimmickUI, ModularRobot->EquippedBodyParts()))
	{
		return true;
	}

	return false;
}
