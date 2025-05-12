#include "ItemGimmickSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Gimmick/Data/BaseDataInfo.h"
#include "Gimmick/GimmickObjects/BaseGimmick/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemDataTable(FSoftObjectPath(TEXT("/Game/GimmickTest/Data/DT_ItemData.DT_ItemData")))
	  , GimmickDataTable(FSoftObjectPath(TEXT("/Game/GimmickTest/Data/DT_GimmickData.DT_GimmickData")))
	  , bIsItemDataTableLoaded(false)
	  , bIsGimmickDataTableLoaded(false)
	  , bIsAllDataTableLoaded(false)
{
}

void UItemGimmickSubsystem::LoadDataTableAsync()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	UE_LOG(LogTemp, Warning, TEXT("Start ItemDataTable loading..."));
	StreamableManager.RequestAsyncLoad(
		ItemDataTable.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &ThisClass::OnItemDataTableLoaded)
	);

	UE_LOG(LogTemp, Warning, TEXT("Start GimmickDataTable loading..."));
	StreamableManager.RequestAsyncLoad(
		GimmickDataTable.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &ThisClass::OnGimmickDataTableLoaded)
	);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UItemGimmickSubsystem::OnItemDataTableLoaded()
{
	const UDataTable* LoadedTable = Cast<UDataTable>(ItemDataTable.Get());
	if (IsValid(LoadedTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable loaded successfully!"));
		ItemDataTable = ItemDataTable.Get();
		bIsItemDataTableLoaded = true;
		CheckAllDataTableLoaded();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDataTable loaded failed!"));
	}
}

void UItemGimmickSubsystem::OnGimmickDataTableLoaded()
{
	const UDataTable* LoadedTable = Cast<UDataTable>(GimmickDataTable.Get());
	if (IsValid(LoadedTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable loaded successfully!"));
		GimmickDataTable = GimmickDataTable.Get();
		bIsGimmickDataTableLoaded = true;
		CheckAllDataTableLoaded();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GimmickDataTable loaded failed!"));
	}
}

void UItemGimmickSubsystem::CheckAllDataTableLoaded()
{
	bIsAllDataTableLoaded = bIsItemDataTableLoaded && bIsGimmickDataTableLoaded;
}

void UItemGimmickSubsystem::SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation)
{
	if (GimmickDataTable.IsValid())
	{
		const UDataTable* LoadedTable = GimmickDataTable.Get();
		if (IsValid(LoadedTable))
		{
			const FBaseGimmickData* GimmickData = LoadedTable->FindRow<FBaseGimmickData>(
				RowName, FString(TEXT("Load Gimmick Data")));
			if (GimmickData)
			{
				UClass* Class = GimmickData->GimmickClass.LoadSynchronous();
				GetWorld()->SpawnActor<ABaseGimmick>(Class, SpawnLocation, FRotator::ZeroRotator);
				UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable loaded successfully!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GimmickDataTable is invalid"));
		}
	}
}
