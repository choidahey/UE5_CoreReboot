#include "ItemGimmickSubsystem.h"

#include "Gimmick/Data/BaseDataInfo.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemDataTable(nullptr)
	  , GimmickDataTable(nullptr)
{
}

TArray<FName> UItemGimmickSubsystem::GetItemDataRowNames() const
{
	return IsValid(ItemDataTable) ? ItemDataTable->GetRowNames() : TArray<FName>();
}

const FBaseItemData* UItemGimmickSubsystem::FindItemData(const FName& RowName) const
{
	return FindRowFromDataTable<FBaseItemData>(ItemDataTable, RowName, TEXT("Load Item Data"));
}

const FBaseGimmickData* UItemGimmickSubsystem::FindGimmickData(const FName& RowName) const
{
	return FindRowFromDataTable<FBaseGimmickData>(GimmickDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmick(const FName& RowName, const FVector& SpawnLocation) const
{
	if (!IsValid(GimmickDataTable))
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

	UClass* GimmickClass = GimmickData->GimmickClass;
	if (!IsValid(GimmickClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickClass is invalid"));
		return nullptr;
	}

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
