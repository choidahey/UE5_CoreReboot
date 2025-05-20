#include "AnimalStatsSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/DataTable.h"

void UAnimalStatsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	StatsDataTableSoft = TSoftObjectPtr<UDataTable>(FSoftObjectPath(TEXT("/Game/CR4S/_Data/AnimalStatsTable.AnimalStatsTable")));
	StatsDataTable = nullptr;
	StatsCache.Empty();
}

const FAnimalStatsRow* UAnimalStatsSubsystem::GetStatsRow(FName RowName) const
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
		return StatsDataTable->FindRow<FAnimalStatsRow>(RowName, TEXT(""));
	}

	return nullptr;
}

void UAnimalStatsSubsystem::GetStatsRowAsync(FName RowName, TFunction<void(const FAnimalStatsRow*)> Callback)
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

		const FAnimalStatsRow* Row = StatsDataTable->FindRow<FAnimalStatsRow>(RowName, TEXT(""));
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