#pragma once

#include "HelperBotSaveGame.h"
#include "GameFramework/SaveGame.h"
#include "CoreSaveGame.generated.h"


USTRUCT(BlueprintType)
struct CR4S_API FSavedActorData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString ActorClassName;
	
	UPROPERTY()
	FTransform ActorTransform;
	
	UPROPERTY()
	FString SerializedData;
};

UCLASS()
class CR4S_API UCoreSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(SaveGame)
	TMap<FName,FSavedActorData> SavedActorsData;

	UPROPERTY(SaveGame)
	int32 NextUniqueID = 0;
};
