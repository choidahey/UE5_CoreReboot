// Fill out your copyright notice in the Description page of Project Settings.


#include "DataLoaderSubsystem.h"

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




