// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Game/SaveGame/CoreSaveGame.h"
#include "SavableActor.generated.h"


class UBaseSaveData;
class USaveGame;

UINTERFACE(MinimalAPI)
class USavableActor : public UInterface
{
	GENERATED_BODY()
};


class CR4S_API ISavableActor
{
	GENERATED_BODY()

public:

	virtual void SetUniqueSaveID(FName NewID) = 0;
	virtual FName GetUniqueSaveID() = 0;
	virtual void GatherSaveData(FSavedActorData& OutSaveData) = 0;
	virtual void ApplySaveData(FSavedActorData& InSaveData) = 0;
};
