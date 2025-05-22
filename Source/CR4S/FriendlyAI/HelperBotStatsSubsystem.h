#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "Engine/StreamableManager.h"
#include "Data/HelperBotStatsRow.h"
#include "HelperBotStatsSubsystem.generated.h"

UCLASS()
class CR4S_API UHelperBotStatsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void GetStatsRowAsync(FName RowName, TFunction<void(const FHelperBotStatsRow*)> Callback);
	const FHelperBotStatsRow* GetStatsRow(FName RowName) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UDataTable> StatsDataTableSoft;

	UPROPERTY(Transient)
	mutable UDataTable* StatsDataTable = nullptr;

	UPROPERTY()
	TMap<FName, FHelperBotStatsRow> StatsCache;

	FStreamableManager StreamableManager;
};
