#pragma once

#include "CoreMinimal.h"
#include "InventorySaveGame.h"
#include "GameFramework/Actor.h"
#include "GimmickSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FGimmickSaveGameData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|SaveGame")
	TSubclassOf<AActor> GimmickClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|SaveGame")
	FTransform Transform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|SaveGame")
	FInventorySaveGame InventorySaveGame;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|SaveGame")
	TArray<FString> SaveValues;
};

USTRUCT(BlueprintType)
struct FGimmickSaveGame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gimmick|SaveGame")
	TArray<FGimmickSaveGameData> GimmickSaveGameDataArray;
};