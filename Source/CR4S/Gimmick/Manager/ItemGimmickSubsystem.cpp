#include "ItemGimmickSubsystem.h"

#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemInfoDataTable(nullptr)
	  , GimmickDataTable(nullptr)
{
	ItemInfoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CR4S/_Data/Item/DT_ItemInfoData.DT_ItemInfoData"));
	GimmickDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CR4S/_Data/Item/DT_GimmickData.DT_GimmickData"));
}

TArray<FName> UItemGimmickSubsystem::GetItemDataRowNames() const
{
	return IsValid(ItemInfoDataTable) ? ItemInfoDataTable->GetRowNames() : TArray<FName>();
}

const FItemInfoData* UItemGimmickSubsystem::FindItemInfoData(const FName& RowName) const
{
	return FindDataFromDataTable<FItemInfoData>(ItemInfoDataTable, RowName, TEXT("Load Item Data"));
}

const FBaseGimmickData* UItemGimmickSubsystem::FindGimmickData(const FName& RowName) const
{
	return FindDataFromDataTable<FBaseGimmickData>(GimmickDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmick(const FName& RowName, const FVector& SpawnLocation) const
{
	if (!IsValid(GimmickDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const FBaseGimmickData* GimmickData
		= GimmickDataTable->FindRow<FBaseGimmickData>(RowName, FString(TEXT("Load Gimmick Data")));
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
