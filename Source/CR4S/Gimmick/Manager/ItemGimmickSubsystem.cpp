#include "ItemGimmickSubsystem.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Gimmick/Data/BaseDataInfo.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: DefaultItemDataTablePath(TEXT("/Game/CR4S/_Data/Item/DT_ItemData.DT_ItemData"))
	  , DefaultGimmickDataTablePath(TEXT("/Game/CR4S/_Data/Item/DT_GimmickData.DT_GimmickData"))
	  , ItemDataTable(DefaultItemDataTablePath)
	  , GimmickDataTable(DefaultGimmickDataTablePath)
	  , bIsItemDataTableLoaded(false)
	  , bIsGimmickDataTableLoaded(false)
{
}

void UItemGimmickSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UItemGimmickSubsystem::Deinitialize()
{
	UnloadAllDataTables();
	
	Super::Deinitialize();
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

UTexture2D* UItemGimmickSubsystem::LoadIcon(const TSoftObjectPtr<UTexture2D>& IconRef)
{
	if (!IconRef.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("IconRefPath is invalid"));
		return nullptr;
	}

	UTexture2D* LoadedIcon = IconRef.LoadSynchronous();
	if (IsValid(LoadedIcon))
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon loaded successfully!"));
		LoadedPaths.Add(IconRef.ToSoftObjectPath());
	}

	return LoadedIcon;
}

void UItemGimmickSubsystem::UnloadAllDataTables()
{
	// Unload ItemDataTable
	if (ItemDataTable.IsValid())
	{
		const FSoftObjectPath ItemDataTablePath = ItemDataTable.ToSoftObjectPath();
		UAssetManager::GetStreamableManager().Unload(ItemDataTablePath);
		ItemDataTable = nullptr;
		ItemDataTable = DefaultItemDataTablePath;
	}

	// Unload GimmickDataTable
	if (GimmickDataTable.IsValid())
	{
		const FSoftObjectPath GimmickDataTablePath = GimmickDataTable.ToSoftObjectPath();
		UAssetManager::GetStreamableManager().Unload(GimmickDataTablePath);
		GimmickDataTable = nullptr;
		GimmickDataTable = DefaultGimmickDataTablePath;
	}

	// Unload all loaded paths
	for (const FSoftObjectPath& LoadedPath : LoadedPaths)
	{
		UAssetManager::GetStreamableManager().Unload(LoadedPath);
	}

	LoadedPaths.Empty();
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
		UE_LOG(LogTemp, Warning, TEXT("%s loaded failed!"), *DataTableName);
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

TArray<FName> UItemGimmickSubsystem::GetItemDataRowNames() const
{
	return ItemDataTable.IsValid() ? ItemDataTable->GetRowNames() : TArray<FName>();
}

const FBaseItemData* UItemGimmickSubsystem::FindItemData(const FName& RowName) const
{
	return FindRowFromDataTable<FBaseItemData>(ItemDataTable, RowName, TEXT("Load Item Data"));
}

const FBaseGimmickData* UItemGimmickSubsystem::FindGimmickData(const FName& RowName) const
{
	return FindRowFromDataTable<FBaseGimmickData>(GimmickDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation)
{
	if (!GimmickDataTable.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const UDataTable* LoadedTable = GimmickDataTable.Get();
	if (!IsValid(LoadedTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const FBaseGimmickData* GimmickData
		= LoadedTable->FindRow<FBaseGimmickData>(RowName, FString(TEXT("Load Gimmick Data")));
	if (!GimmickData)
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickData is invalid"));
		return nullptr;
	}
	
	UClass* GimmickClass = GimmickData->GimmickClass.Get();
	if (!IsValid(GimmickClass))
	{
		GimmickClass = GimmickData->GimmickClass.LoadSynchronous();
	}

	if (!IsValid(GimmickClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickClass is invalid"));
		return nullptr;
	}

	LoadedPaths.Add(GimmickData->GimmickClass.ToSoftObjectPath());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseGimmick* Gimmick
		= GetWorld()->SpawnActor<ABaseGimmick>(GimmickClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (!IsValid(Gimmick))
	{
		UE_LOG(LogTemp, Warning, TEXT("Gimmick is invalid"));
		return nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Gimmick spawned successfully!"));
	return Gimmick;
}
