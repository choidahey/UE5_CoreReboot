#include "HelperBotStatsSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"

void UHelperBotStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	StatsDataTableSoft = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/CR4S/_Data/HelperBot/HelperBotStatsTable.HelperBotStatsTable")));
	StatsDataTable = nullptr;
	StatsCache.Empty();
}

const FHelperBotStatsRow* UHelperBotStatsSubsystem::GetStatsRow(FName RowName) const
{
	if (StatsCache.Contains(RowName))
	{
		return &StatsCache[RowName];
	}

	if (!StatsDataTable && StatsDataTableSoft.IsValid())
	{
		StatsDataTable = StatsDataTableSoft.Get();
	}

	if (StatsDataTable)
	{
		return StatsDataTable->FindRow<FHelperBotStatsRow>(RowName, TEXT(""));
	}

	return nullptr;
}

void UHelperBotStatsSubsystem::GetStatsRowAsync(FName RowName, TFunction<void(const FHelperBotStatsRow*)> Callback)
{
	if (StatsCache.Contains(RowName))
	{
		Callback(&StatsCache[RowName]);
		return;
	}

	FSoftObjectPath AssetPath = StatsDataTableSoft.ToSoftObjectPath();
	StreamableManager.RequestAsyncLoad(AssetPath, [this, RowName, Callback]()
	{
		StatsDataTable = StatsDataTableSoft.Get();
		if (!StatsDataTable)
		{
			Callback(nullptr);
			return;
		}

		const FHelperBotStatsRow* Row = StatsDataTable->FindRow<FHelperBotStatsRow>(RowName, TEXT(""));
		if (Row)
		{
			StatsCache.Add(RowName, *Row);
			Callback(&StatsCache[RowName]);
		}
		else
		{
			Callback(nullptr);
		}
	});
}
