// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	TSubclassOf<APlayerCharacter> PlayerCharacterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<AModularRobot> ModularRobotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	TSubclassOf<ABaseHelperBot> HelperBotClass;
};
