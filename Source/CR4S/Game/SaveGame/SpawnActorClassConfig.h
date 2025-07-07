// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Engine/DataAsset.h"
#include "SpawnActorClassConfig.generated.h"

class AModularRobot;
class APlayerCharacter;
class ABaseHelperBot;
/**
 * 
 */
UCLASS()
class CR4S_API USpawnActorClassConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TMap<FString, TSubclassOf<AActor>> SpawnableClasses
	{
		{"APlayerCharacter",nullptr}	,
		{"AModularRobot",nullptr}	,
		{"ABaseHelperBot",nullptr}	
	};

	UFUNCTION(BlueprintPure, Category = "Spawn")
	TSubclassOf<AActor> GetSpawnClassByName(const FString& ClassName) const
	{
		if (const TSubclassOf<AActor>* FoundClass = SpawnableClasses.Find(ClassName))
		{
			return *FoundClass;
		}
		return nullptr;
	}
};
