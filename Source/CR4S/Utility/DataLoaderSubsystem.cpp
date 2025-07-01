// Fill out your copyright notice in the Description page of Project Settings.


#include "DataLoaderSubsystem.h"

#include "Character/Data/RobotPartsData.h"
#include "Character/Data/RobotSettings.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"

void UDataLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (const UCR4SDataTableSettings* Settings=GetDefault<UCR4SDataTableSettings>())
	{
		WeaponDataAsset=Cast<UWeaponInfoDataAsset>(Settings->GetDataAssetByName(TEXT("WeaponInfoDataAsset")));
		RobotSettingsDataAsset=Cast<URobotSettingsDataAsset>(Settings->GetDataAssetByName(TEXT("RobotSettingsDataAsset")));
		PlayerToolInfoDataAsset=Cast<UPlayerToolInfoDataAsset>(Settings->GetDataAssetByName(TEXT("PlayerToolInfoDataAsset")));
		WeaponClassDataAsset=Cast<UWeaponClassDataAsset>(Settings->GetDataAssetByName(TEXT("WeaponClassDataAsset")));
		CorePartsDataAsset=Cast<UCorePartsData>(Settings->GetDataAssetByName(TEXT("CorePartsDataAsset")));
		BodyPartsDataAsset=Cast<UBodyPartsData>(Settings->GetDataAssetByName(TEXT("BodyPartsDataAsset")));
		ArmPartsDataAsset=Cast<UArmPartsData>(Settings->GetDataAssetByName(TEXT("ArmPartsDataAsset")));
		LegPartsDataAsset=Cast<ULegPartsData>(Settings->GetDataAssetByName(TEXT("LegPartsDataAsset")));
		BoosterPartsDataAsset=Cast<UBoosterPartsData>(Settings->GetDataAssetByName(TEXT("BoosterPartsDataAsset")));
	}
}

void UDataLoaderSubsystem::LoadRobotSettingsData(FRobotSettings& OutSettingsInfo) const
{
	OutSettingsInfo=RobotSettingsDataAsset->RobotSettings;
}

void UDataLoaderSubsystem::LoadWeaponSettingsData(FWeaponSettings& OutSettingsInfo) const
{
	OutSettingsInfo=RobotSettingsDataAsset->WeaponSettings;
}

bool UDataLoaderSubsystem::LoadToolInfoByTag(const FGameplayTag& Tag, FPlayerToolInfo& OutSettingsInfo) const
{
	const FPlayerToolInfo* Info=PlayerToolInfoDataAsset->PlayerToolInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,Info)) return false;
	
	OutSettingsInfo=*Info;
	return true;
}

bool UDataLoaderSubsystem::LoadWeaponClassDataByTag(const FGameplayTag& Tag, TSubclassOf<ABaseWeapon>& OutWeaponClass) const
{
	const TSubclassOf<ABaseWeapon>* WeaponClass=WeaponClassDataAsset->WeaponClassData.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,WeaponClass)) return false;

	OutWeaponClass=*WeaponClass;
	return true;
}

bool UDataLoaderSubsystem::LoadCorePartsDataByTag(const FGameplayTag& Tag, FCorePartsInfo& OutPartsInfo) const
{
	const FCorePartsInfo* PartsInfo=CorePartsDataAsset->CoreInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,PartsInfo)) return false;

	OutPartsInfo=*PartsInfo;
	return true;
}

bool UDataLoaderSubsystem::LoadBodyPartsDataByTag(const FGameplayTag& Tag, FBodyPartsInfo& OutPartsInfo) const
{
	const FBodyPartsInfo* PartsInfo=BodyPartsDataAsset->BodyInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,PartsInfo)) return false;

	OutPartsInfo=*PartsInfo;
	return true;
}

bool UDataLoaderSubsystem::LoadArmPartsDataByTag(const FGameplayTag& Tag, FArmPartsInfo& OutPartsInfo) const
{
	const FArmPartsInfo* PartsInfo=ArmPartsDataAsset->ArmInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,PartsInfo)) return false;

	OutPartsInfo=*PartsInfo;
	return true;
}

bool UDataLoaderSubsystem::LoadLegPartsDataByTag(const FGameplayTag& Tag, FLegPartsInfo& OutPartsInfo) const
{
	const FLegPartsInfo* PartsInfo=LegPartsDataAsset->LegInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,PartsInfo)) return false;

	OutPartsInfo=*PartsInfo;
	return true;
}

bool UDataLoaderSubsystem::LoadBoosterPartsDataByTag(const FGameplayTag& Tag, FBoosterPartsInfo& OutPartsInfo) const
{
	const FBoosterPartsInfo* PartsInfo=BoosterPartsDataAsset->BoosterInfo.Find(Tag);
	if (!CR4S_ENSURE(LogHong1,PartsInfo)) return false;

	OutPartsInfo=*PartsInfo;
	return true;
}




