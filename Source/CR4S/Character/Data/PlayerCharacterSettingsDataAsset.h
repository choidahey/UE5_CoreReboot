// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCharacterSettingsDataAsset.generated.h"

USTRUCT(BlueprintType)
struct CR4S_API FPlayerCharacterSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MappingContextPriority{2};
};

UCLASS()
class CR4S_API UPlayerCharacterSettingsDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerCharacterSettings PlayerCharacterSettings;
};
