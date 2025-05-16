#include "ItemGimmickSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Gimmick/Data/BaseDataInfo.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemDataTable(FSoftObjectPath(TEXT("/Game/CR4S/_Data/Item/DT_ItemData.DT_ItemData")))
	  , GimmickDataTable(FSoftObjectPath(TEXT("/Game/CR4S/_Data/Item/DT_GimmickData.DT_GimmickData")))
	  , bIsItemDataTableLoaded(false)
	  , bIsGimmickDataTableLoaded(false)
{
}

void UItemGimmickSubsystem::LoadDataTableAsync()
{
	FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();

	if (ItemDataTable.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start ItemDataTable loading..."));
		StreamableManager.RequestAsyncLoad(
			ItemDataTable.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnItemDataTableLoaded)
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTable ReferencePath is invalid"));
	}

	if (GimmickDataTable.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start GimmickDataTable loading..."));
		StreamableManager.RequestAsyncLoad(
			GimmickDataTable.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnGimmickDataTableLoaded)
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable ReferencePath is invalid"));
	}
}

void UItemGimmickSubsystem::OnDataTableLoaded(TSoftObjectPtr<UDataTable>& DataTable,
                                              bool& bIsDataTableLoaded, const FString& DataTableName) const
{
	const UDataTable* LoadedTable = Cast<UDataTable>(DataTable.Get());
	if (IsValid(LoadedTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s loaded successfully!"), *DataTableName);
		DataTable = DataTable.Get();
		bIsDataTableLoaded = true;
		CheckAllDataTableLoaded();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s loaded failed!"), *DataTableName);
	}
}

void UItemGimmickSubsystem::OnItemDataTableLoaded()
{
	OnDataTableLoaded(ItemDataTable, bIsItemDataTableLoaded, TEXT("ItemDataTable"));
}

void UItemGimmickSubsystem::OnGimmickDataTableLoaded()
{
	OnDataTableLoaded(GimmickDataTable, bIsGimmickDataTableLoaded, TEXT("GimmickDataTable"));
}

void UItemGimmickSubsystem::CheckAllDataTableLoaded() const
{
	if (bIsItemDataTableLoaded && bIsGimmickDataTableLoaded)
	{
		OnDataTablesReady.Broadcast();
	}
}

const FBaseItemData* UItemGimmickSubsystem::FindItemData(const FName& RowName) const
{
	return FindRow<FBaseItemData>(ItemDataTable, RowName, TEXT("Load Item Data"));
}

const FBaseGimmickData* UItemGimmickSubsystem::FindGimmickData(const FName& RowName) const
{
	return FindRow<FBaseGimmickData>(GimmickDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation) const
{
	if (!GimmickDataTable.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const UDataTable* LoadedTable = GimmickDataTable.Get();
	if (!IsValid(LoadedTable))
	{
		UE_LOG(LogTemp, Error, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const FBaseGimmickData* GimmickData
		= LoadedTable->FindRow<FBaseGimmickData>(RowName, FString(TEXT("Load Gimmick Data")));
	if (!GimmickData)
	{
		UE_LOG(LogTemp, Error, TEXT("GimmickData is invalid"));
		return nullptr;
	}

	UClass* Class = GimmickData->GimmickClass.LoadSynchronous();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseGimmick* Gimmick
		= GetWorld()->SpawnActor<ABaseGimmick>(Class, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!IsValid(Gimmick))
	{
		UE_LOG(LogTemp, Error, TEXT("Gimmick is invalid"));
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Gimmick spawned successfully!"));
	return Gimmick;
}
