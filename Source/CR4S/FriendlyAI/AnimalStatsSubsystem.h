#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Data/AnimalStatsRow.h"
#include "AnimalStatsSubsystem.generated.h"


UCLASS()
class CR4S_API UAnimalStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void GetStatsRowAsync(FName RowName, TFunction<void(const FAnimalStatsRow*)> Callback);

	const FAnimalStatsRow* GetStatsRow(FName RowName) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UDataTable> StatsDataTableSoft;

	UPROPERTY(Transient)
	mutable UDataTable* StatsDataTable = nullptr;

	UPROPERTY()
	TMap<FName, FAnimalStatsRow> StatsCache;

	FStreamableManager StreamableManager;
};