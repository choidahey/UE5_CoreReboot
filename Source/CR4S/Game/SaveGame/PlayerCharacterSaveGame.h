// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventorySaveGame.h"
#include "PlayerCharacterSaveGame.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FPlayerCharacterSaveGame
{
	GENERATED_BODY()
public:
	FPlayerCharacterSaveGame() {}
	

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentHP{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentResource{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentHunger{0};

	UPROPERTY(VisibleAnywhere, Category = "SaveData | EnvStatus")
	float CurrentTemperature{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | EnvStatus")
	float CurrentHumidity{0};
	
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Equipment")
	FGameplayTag EquippedToolTag;

	UPROPERTY(VisibleAnywhere, Category = "SaveData | ALS")
	FGameplayTag Stance;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | ALS")
	FGameplayTag Gait;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | ALS")
	FGameplayTag OverlayMode;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | ALS")
	uint8 bIsRightShoulder:1 {true};

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Inventory")
	FInventorySaveGame InventorySaveGame;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Inventory")
	FInventorySaveGame QuickSlotSaveGame;
};

USTRUCT(BlueprintType)
struct CR4S_API FRobotWeaponSaveGame
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	FGameplayTag WeaponTag;
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	int32 CurrentAmmo{0};
};

USTRUCT(BlueprintType)
struct CR4S_API FModularRobotSaveGame
{
	GENERATED_BODY()
public:
	FModularRobotSaveGame() {}
	

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentHP{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentResource{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentEnergy{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Status")
	float CurrentStun{0};

	UPROPERTY(VisibleAnywhere, Category = "SaveData | EnvStatus")
	float CurrentTemperature{0};
	UPROPERTY(VisibleAnywhere, Category = "SaveData | EnvStatus")
	float CurrentHumidity{0};

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Parts")
	FGameplayTag CoreTag;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Parts")
	FGameplayTag BodyTag;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Parts")
	FGameplayTag ArmTag;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Parts")
	FGameplayTag LegTag;
	UPROPERTY(VisibleAnywhere, Category = "SaveData | Parts")
	FGameplayTag BoosterTag;

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Weapons")
	TArray<FRobotWeaponSaveGame> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, Category = "SaveData | Mount")
	uint8 bWasPlayerMounted:1 {false};
};