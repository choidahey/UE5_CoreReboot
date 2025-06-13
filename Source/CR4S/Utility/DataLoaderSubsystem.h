// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CR4S.h"
#include "Character/Data/WeaponData.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataLoaderSubsystem.generated.h"

struct FWeaponSettings;
struct FRobotSettings;
class URobotSettingsDataAsset;
class UWeaponInfoDataAsset;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class CR4S_API UDataLoaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadRobotSettingsData(FRobotSettings& OutSettingsInfo) const;
	void LoadWeaponSettingsData(FWeaponSettings& OutSettingsInfo) const;
	bool LoadToolInfoByTag(const FGameplayTag& Tag, FPlayerToolInfo& OutSettingsInfo) const;
	
	template<typename T>
	bool LoadWeaponInfoByTag(const FGameplayTag& Tag, T& OutInfo, FBaseWeaponInfo& OutBaseInfo)
	{
		if constexpr (std::is_same_v<T, FMeleeWeaponInfo>)
		{
			if (const FMeleeWeaponInfo* Info = WeaponDataAsset->MeleeInfo.Find(Tag))
			{
				CR4S_ENSURE(LogHong1,Info!=nullptr);
				OutInfo = *Info;
			}
		}
		else if constexpr (std::is_same_v<T, FRangedWeaponInfo>)
		{
			if (const FRangedWeaponInfo* Info = WeaponDataAsset->RangedInfo.Find(Tag))
			{
				CR4S_ENSURE(LogHong1,Info!=nullptr);
				OutInfo = *Info;
			}
		}
		if (const FBaseWeaponInfo* Info=WeaponDataAsset->BaseInfo.Find(Tag))
		{
			CR4S_ENSURE(LogHong1,Info!=nullptr);
			OutBaseInfo = *Info;
			return true;
		}
		return false;
	}
	
private:
	UPROPERTY()
	TObjectPtr<UWeaponInfoDataAsset> WeaponDataAsset;

	UPROPERTY()
	TObjectPtr<URobotSettingsDataAsset> RobotSettingsDataAsset;

	UPROPERTY()
	TObjectPtr<UPlayerToolInfoDataAsset> PlayerToolInfoDataAsset;
};
