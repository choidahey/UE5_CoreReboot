#include "ItemGimmickSubsystem.h"

#include "CR4S.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemInfoDataTable(nullptr)
	  , GimmickInfoDataTable(nullptr)
{
	// ItemInfoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CR4S/_Data/Item/DT_ItemInfoData.DT_ItemInfoData"));
	// GimmickDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/CR4S/_Data/Item/DT_GimmickData.DT_GimmickData"));
}

void UItemGimmickSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const UCR4SDataTableSettings* Settings = GetDefault<UCR4SDataTableSettings>();
	if (!IsValid(Settings))
	{
		CR4S_Log(LogTemp, Warning, TEXT("Settings is invalid"));
		return;
	}
	
	ItemInfoDataTable = Settings->GetDataTableByName(TEXT("ItemInfoData"));
	GimmickInfoDataTable = Settings->GetDataTableByName(TEXT("GimmickInfoData"));
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
	return FindDataFromDataTable<FBaseGimmickData>(GimmickInfoDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmick(const FName& RowName, const FVector& SpawnLocation) const
{
	if (!IsValid(GimmickInfoDataTable))
	{
		UE_LOG(LogTemp, Warning, TEXT("GimmickDataTable is invalid"));
		return nullptr;
	}

	const FBaseGimmickData* GimmickData
		= GimmickInfoDataTable->FindRow<FBaseGimmickData>(RowName, FString(TEXT("Load Gimmick Data")));
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
