#pragma once

#include "PlayerCharacterSaveGame.h"
#include "GameFramework/SaveGame.h"
#include "CoreSaveGame.generated.h"

UENUM(BlueprintType)
enum class ESavedActorType : uint8
{
	None,
	PlayerCharacter,
	ModularRobot
};

USTRUCT(BlueprintType)
struct CR4S_API FSavedActorData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	ESavedActorType ActorType = ESavedActorType::None;
	
	UPROPERTY()
	FTransform ActorTransform;
	
	UPROPERTY()
	FPlayerCharacterSaveGame PlayerCharacterData;

	UPROPERTY()
	FModularRobotSaveGame RobotData;
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
