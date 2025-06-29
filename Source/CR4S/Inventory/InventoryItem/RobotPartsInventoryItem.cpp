#include "RobotPartsInventoryItem.h"

#include "Character/Data/RobotPartsData.h"
#include "Utility/DataLoaderSubsystem.h"

#define LOCTEXT_NAMESPACE "RobotPartsInventoryItem"

void URobotPartsInventoryItem::InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
                                                 const FInventoryItemData& NewInventoryItemData,
                                                 const int32 StackCount)
{
	Super::InitInventoryItem(NewInventoryComponent, NewInventoryItemData, StackCount);

	InitTextFormat();
}

void URobotPartsInventoryItem::InitTextFormat()
{
	const UDataTable* DataTable = InventoryItemData.ItemInfoData.DetailData.DataTable;
	if (!IsValid(DataTable))
	{
		return;
	}

	const FName RowName = InventoryItemData.ItemInfoData.DetailData.RowName;

	if (const FRobotPartsInfoFormatData* RobotPartsInfoFormatData
		= DataTable->FindRow<FRobotPartsInfoFormatData>(RowName, TEXT("")))
	{
		const FString FormatString = RobotPartsInfoFormatData->FormatPattern.ToString();
		TextFormat = FTextFormat::FromString(FormatString);

		InitRobotPartsInfoData(InventoryItemData.ItemInfoData.ItemTags.First());
	}
}

void URobotPartsInventoryItem::InitRobotPartsInfoData(const FGameplayTag& Tag)
{
	const UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(GameInstance)))
	{
		return;
	}

	UDataLoaderSubsystem* Loader = GameInstance->GetSubsystem<UDataLoaderSubsystem>();
	if (!CR4S_VALIDATE(LogInventoryItem, IsValid(Loader)))
	{
		return;
	}

	if (Tag.MatchesTag(RobotParts::Body))
	{
		FBodyPartsInfo BodyPartsInfo;
		const bool bSucceed = Loader->LoadBodyPartsDataByTag(Tag, BodyPartsInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(BodyPartsInfo.MaxHealth),
			FText::AsNumber(BodyPartsInfo.Armor),
			FText::AsNumber(BodyPartsInfo.MaxStun),
			FText::AsNumber(BodyPartsInfo.Weight),
			FText::AsNumber(BodyPartsInfo.EnergyEfficiency),
			FText::AsNumber(BodyPartsInfo.ResourceRegenModifier),
			FText::AsNumber(BodyPartsInfo.ResourceRegenDelay),
			FText::AsNumber(BodyPartsInfo.ResourceConsumptionModifier),
			FText::AsNumber(BodyPartsInfo.HeatThreshold),
			FText::AsNumber(BodyPartsInfo.ColdThreshold),
			FText::AsNumber(BodyPartsInfo.HumidityThreshold)
		);
	}
	else if (Tag.MatchesTag(RobotParts::Arm))
	{
		FArmPartsInfo ArmPartsInfo;
		const bool bSucceed = Loader->LoadArmPartsDataByTag(Tag, ArmPartsInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(ArmPartsInfo.MaxHealth),
			FText::AsNumber(ArmPartsInfo.Armor),
			FText::AsNumber(ArmPartsInfo.Weight),
			FText::AsNumber(ArmPartsInfo.MaxArmLoad),
			FText::AsNumber(ArmPartsInfo.RecoilModifier),
			FText::AsNumber(ArmPartsInfo.MeleeDamageModifier)
		);
	}
	else if (Tag.MatchesTag(RobotParts::Leg))
	{
		FLegPartsInfo LegPartsInfo;
		const bool bSucceed = Loader->LoadLegPartsDataByTag(Tag, LegPartsInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(LegPartsInfo.MaxHealth),
			FText::AsNumber(LegPartsInfo.Armor),
			FText::AsNumber(LegPartsInfo.MaxStun),
			FText::AsNumber(LegPartsInfo.Weight),
			FText::AsNumber(LegPartsInfo.MaxTotalWeight),
			FText::AsNumber(LegPartsInfo.MaxWalkSpeed),
			FText::AsNumber(LegPartsInfo.LegStrength),
			FText::AsNumber(LegPartsInfo.MaxSlopeAngle),
			FText::AsNumber(LegPartsInfo.EnergyEfficiency)
		);
	}
	else if (Tag.MatchesTag(RobotParts::Core))
	{
		FCorePartsInfo CorePartsInfo;
		const bool bSucceed = Loader->LoadCorePartsDataByTag(Tag, CorePartsInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			FText::AsNumber(CorePartsInfo.AttackPower),
			FText::AsNumber(CorePartsInfo.ArmorMultiplier),
			FText::AsNumber(CorePartsInfo.AdditiveTemperatureThreshold),
			FText::AsNumber(CorePartsInfo.AdditiveHumidityThreshold),
			FText::AsNumber(CorePartsInfo.MaxResource),
			FText::AsNumber(CorePartsInfo.EnergyConsumptionAmount),
			FText::AsNumber(CorePartsInfo.MaxStun)
		);
	}
	else if (Tag.MatchesTag(RobotParts::Booster))
	{
		FBoosterPartsInfo BoosterPartsInfo;
		const bool bSucceed = Loader->LoadBoosterPartsDataByTag(Tag, BoosterPartsInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(BoosterPartsInfo.BoosterStrength),
			FText::AsNumber(BoosterPartsInfo.ResourceConsumption),
			FText::AsNumber(BoosterPartsInfo.DashCooldown)
		);
	}
	else if (Tag.MatchesTag(WeaponTags::Melee))
	{
		FBaseWeaponInfo BaseWeaponInfo;
		FMeleeWeaponInfo MeleeWeaponInfo;
		const bool bSucceed = Loader->LoadWeaponInfoByTag<FMeleeWeaponInfo>(Tag, MeleeWeaponInfo, BaseWeaponInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(BaseWeaponInfo.DamageMultiplier),
			FText::AsNumber(BaseWeaponInfo.Weight),
			FText::AsNumber(BaseWeaponInfo.AttackCooldownTime),
			FText::FromString(BaseWeaponInfo.bHasSelfStun ? FString("TRUE") : FString("FALSE")),
			FText::AsNumber(MeleeWeaponInfo.StunAmount),
			FText::AsNumber(MeleeWeaponInfo.MaxCombo)
		);
	}
	else if (Tag.MatchesTag(WeaponTags::Ranged))
	{
		FBaseWeaponInfo BaseWeaponInfo;
		FRangedWeaponInfo RangedWeaponInfo;
		const bool bSucceed = Loader->LoadWeaponInfoByTag<FRangedWeaponInfo>(Tag, RangedWeaponInfo, BaseWeaponInfo);

		if (!CR4S_VALIDATE(LogInventoryItem, bSucceed))
		{
			return;
		}

		InventoryItemData.ItemInfoData.Description = FText::Format(
			TextFormat,
			InventoryItemData.ItemInfoData.Description,
			FText::AsNumber(BaseWeaponInfo.DamageMultiplier),
			FText::AsNumber(BaseWeaponInfo.Weight),
			FText::AsNumber(BaseWeaponInfo.AttackCooldownTime),
			FText::FromString(BaseWeaponInfo.bHasSelfStun ? FString("TRUE") : FString("FALSE")),
			FText::AsNumber(RangedWeaponInfo.MaxAimTrackingRange),
			FText::AsNumber(RangedWeaponInfo.AmmoInfo.MagazineCapacity),
			FText::AsNumber(RangedWeaponInfo.AutomaticFireInfo.ShotsPerSecond),
			FText::AsNumber(RangedWeaponInfo.AmmoInfo.ReloadTime),
			FText::AsNumber(RangedWeaponInfo.BulletInfo.MaxBulletSpeed),
			FText::AsNumber(RangedWeaponInfo.RecoilInfo.Recoil)
		);
	}
}


#define LOCTEXT_NAMESPACE "RobotPartsInventoryItem"
