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
	}
	if (const UCR4SDataTableSettings* Settings=GetDefault<UCR4SDataTableSettings>())
	{
		RobotSettingsDataAsset=Cast<URobotSettingsDataAsset>(Settings->GetDataAssetByName(TEXT("RobotSettingsDataAsset")));
	}
}

void UDataLoaderSubsystem::LoadRobotSettingsData(FRobotSettings& OutSettingsInfo) const
{
	OutSettingsInfo=RobotSettingsDataAsset->RobotSettings;
}
