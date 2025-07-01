#pragma once

#include "GameFramework/SaveGame.h"
#include "Game/System/SeasonType.h"
#include "GimmickSaveGame.h"
#include "Game/System/WorldTimeManager.h"
#include "WorldSaveGame.generated.h"

UCLASS()
class CR4S_API UWorldSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere, Category = "Season")
	ESeasonType Season;	
	UPROPERTY(VisibleAnywhere, Category = "Season")
	int32 SeasonDay;

	UPROPERTY(VisibleAnywhere, Category = "Time")
	float DawnTime;
	UPROPERTY(VisibleAnywhere, Category = "Time")
	float DuskTime;
	UPROPERTY(VisibleAnywhere, Category = "Time")
	FWorldTimeData TimeData;
	UPROPERTY(VisibleAnywhere, Category = "Time")
	int64 TotalGameTime;

	UPROPERTY(VisibleAnywhere, Category = "DifficultySettings")
	int32 SeasonLength;
	UPROPERTY(VisibleAnywhere, Category = "DifficultySettings")
	int32 DayCycleLength;

	UPROPERTY(VisibleAnywhere, Category = "Item|Gimmick")
	FGimmickSaveGame GimmickSaveGame;
};
