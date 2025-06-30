#include "ItemGimmickSubsystem.h"

#include "CR4S.h"
#include "DeveloperSettings/CR4SDataTableSettings.h"
#include "Game/SaveGame/GimmickSaveGame.h"
#include "Game/System/ProjectilePoolSubsystem.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "Gimmick/GimmickObjects/ItemPouchGimmick.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "Inventory/InventoryItem/ConsumableInventoryItem.h"
#include "Inventory/InventoryItem/HelperBotInventoryItem.h"
#include "Inventory/InventoryItem/RobotPartsInventoryItem.h"
#include "Inventory/InventoryItem/ToolInventoryItem.h"
#include "Kismet/GameplayStatics.h"
#include "Utility/Cr4sGameplayTags.h"

UItemGimmickSubsystem::UItemGimmickSubsystem()
	: ItemInfoDataTable(nullptr)
	  , GimmickInfoDataTable(nullptr)
{
}

bool UItemGimmickSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	if (!CR4S_VALIDATE(LogGimmick, IsValid(World)))
	{
		return false;
	}
	
	if (World->GetName().Contains("MenuLevel"))
	{
		return false;
	}

	CR4S_Log(LogGimmick, Warning, TEXT("Create ItemGimmickSubsystem for %s"), *World->GetName());
	return true;
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

void UItemGimmickSubsystem::PostInitialize()
{
	Super::PostInitialize();

	PoolSubsystem = GetWorld()->GetSubsystem<UProjectilePoolSubsystem>();
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

void UItemGimmickSubsystem::GetGimmickInfoData(const FName& RowName, FGimmickInfoData& OutGimmickInfoData) const
{
	OutGimmickInfoData = *FindGimmickInfoData(RowName);
}

UBaseInventoryItem* UItemGimmickSubsystem::CreateInventoryItem(UObject* Outer, const FGameplayTagContainer& ItemTags)
{
	if (ItemTags.HasTag(ItemTags::Tools))
	{
		return NewObject<UToolInventoryItem>(Outer);
	}
	
	if (ItemTags.HasTag(ItemTags::Consumable))
	{
		return NewObject<UConsumableInventoryItem>(Outer);
	}
	
	if (ItemTags.HasTag(ItemTags::HelperBot))
	{
		return NewObject<UHelperBotInventoryItem>(Outer);
	}

	if (ItemTags.HasTag(ItemTags::RobotParts) || ItemTags.HasTag(ItemTags::Weapon))
	{
		return NewObject<URobotPartsInventoryItem>(Outer);
	}

	return NewObject<UBaseInventoryItem>(Outer);
}

void UItemGimmickSubsystem::SpawnItemPouch(const AActor* SourceActor, const TMap<FName, int32>& RemainingItems,
                                           const float ForwardOffset)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(SourceActor)))
	{
		return;
	}

	const FVector ForwardOffsetVector = SourceActor->GetActorForwardVector() + ForwardOffset;

	const AItemPouchGimmick* ItemPouch
		= SpawnGimmickByRowName<AItemPouchGimmick>("ItemPouch",
		                                           SourceActor->GetActorLocation() + ForwardOffsetVector,
		                                           SourceActor->GetActorRotation());

	ItemPouch->InitItemPouch(SourceActor, RemainingItems);
}

ABaseGimmick* UItemGimmickSubsystem::SpawnGimmick(const FName& RowName, const FVector& SpawnLocation,
                                                  const FRotator& SpawnRotation) const
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
	//
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.SpawnCollisionHandlingOverride =
	// 	ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	//
	// ABaseGimmick* Gimmick
	// 	= GetWorld()->SpawnActor<ABaseGimmick>(GimmickClass, SpawnLocation, SpawnRotation, SpawnParams);

	AActor* SpawnedActor
				= PoolSubsystem->SpawnFromPool(GimmickClass,
											   SpawnLocation,
											   SpawnRotation);

	ABaseGimmick* Gimmick = Cast<ABaseGimmick>(SpawnedActor);
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Gimmick)))
	{
		return nullptr;
	}

	CR4S_Log(LogGimmick, Warning, TEXT("Gimmick spawned successfully!"));

	Gimmick->SetGimmickDataRowName(RowName);
	return Gimmick;
}

FGimmickSaveGame UItemGimmickSubsystem::GetGimmickSaveGame() const
{
	FGimmickSaveGame SaveGame;

	TArray<AActor*> Gimmicks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseGimmick::StaticClass(), Gimmicks);

	for (AActor* GimmickActor : Gimmicks)
	{
		if (GimmickActor->IsHidden())
		{
			continue;
		}

		ABaseGimmick* Gimmick = Cast<ABaseGimmick>(GimmickActor);
		if (IsValid(Gimmick))
		{
			bool bSuccess = false;
			const FGimmickSaveGameData& GimmickSaveGameData = Gimmick->GetGimmickSaveGameData(bSuccess);

			if (bSuccess)
			{
				SaveGame.GimmickSaveGameDataArray.Emplace(GimmickSaveGameData);
			}
		}
	}

	return SaveGame;
}

void UItemGimmickSubsystem::LoadGimmickSaveGame(const FGimmickSaveGame& GimmickSaveGame)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(PoolSubsystem)))
	{
		return;
	}

	TArray<AActor*> Gimmicks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseGimmick::StaticClass(), Gimmicks);
	for (AActor* GimmickActor : Gimmicks)
	{
		ACropsGimmick* CropsGimmick = Cast<ACropsGimmick>(GimmickActor);
		if (IsValid(CropsGimmick) && CropsGimmick->IsPlanted())
		{
			continue;
		}
		
		ABaseGimmick* Gimmick = Cast<ABaseGimmick>(GimmickActor);
		if (IsValid(Gimmick))
		{
			Gimmick->GimmickDestroy();
		}
	}

	TArray<FGimmickSaveGameData> GimmickSaveGameDataArray = GimmickSaveGame.GimmickSaveGameDataArray;
	for (const FGimmickSaveGameData& GimmickSaveGameData : GimmickSaveGameDataArray)
	{
		AActor* SpawnedActor
			= PoolSubsystem->SpawnFromPool(GimmickSaveGameData.GimmickClass,
			                               GimmickSaveGameData.Transform.GetLocation(),
			                               GimmickSaveGameData.Transform.Rotator());
	
		ABaseGimmick* Gimmick = Cast<ABaseGimmick>(SpawnedActor);
		if (IsValid(Gimmick))
		{
			Gimmick->LoadGimmickSaveGameData(GimmickSaveGameData);
		}
	}
}

USaveGame* UItemGimmickSubsystem::GetBuildingSaveGame() const
{
	OnCreateBuildingSaveGame.Broadcast();
	
	return BuildingSaveGame;
}

void UItemGimmickSubsystem::LoadBuildingSaveGame(USaveGame* NewBuildingSaveGame) const
{
	OnLoadBuildingSaveGame.Broadcast(NewBuildingSaveGame);
}
