#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/AnimalStatsRow.h"
#include "AnimalStatsSubsystem.generated.h"

UCLASS()
class CR4S_API UAnimalStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	const FAnimalStatsRow* GetStatsRow(FName RowName) const;

private:
	UPROPERTY()
	UDataTable* StatsDataTable = nullptr;

	UPROPERTY()
	mutable TMap<FName, FAnimalStatsRow> StatsCache;

	const FString LogHeader = TEXT("AnimalStatsSubsystem");
};
