#include "ItemGimmickSubsystem.h"

#include "CR4S.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemInfoDataTable(nullptr)
	  , GimmickInfoDataTable(nullptr)
{
}

void UItemGimmickSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UCR4SDataTableSettings* Settings = GetDefault<UCR4SDataTableSettings>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Settings)))
	{
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

const FGimmickInfoData* UItemGimmickSubsystem::FindGimmickInfoData(const FName& RowName) const
{
	return FindDataFromDataTable<FGimmickInfoData>(GimmickInfoDataTable, RowName, TEXT("Load Gimmick Data"));
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmick(const FName& RowName, const FVector& SpawnLocation, const FRotator& SpawnRotation) const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(GimmickInfoDataTable)))
	{
		return nullptr;
	}

	const FGimmickInfoData* GimmickData
		= GimmickInfoDataTable->FindRow<FGimmickInfoData>(RowName, FString(TEXT("Load Gimmick Data")));
	if (!CR4S_VALIDATE(LogGimmick, GimmickData))
	{
		return nullptr;
	}

	UClass* GimmickClass = GimmickData->GimmickClass;
	if (!CR4S_VALIDATE(LogGimmick, IsValid(GimmickClass)))
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABaseGimmick* Gimmick
		= GetWorld()->SpawnActor<ABaseGimmick>(GimmickClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Gimmick)))
	{
		return nullptr;
	}

	CR4S_Log(LogGimmick, Warning, TEXT("Gimmick spawned successfully!"));
	return Gimmick;
}
