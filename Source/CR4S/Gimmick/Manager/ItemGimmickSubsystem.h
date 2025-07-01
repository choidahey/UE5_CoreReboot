#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/GimmickData.h"
#include "Gimmick/Data/ItemData.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ItemGimmickSubsystem.generated.h"

class UBaseInventoryItem;
class USaveGame;
class UProjectilePoolSubsystem;
struct FGimmickSaveGameData;
struct FGimmickSaveGame;
class ABaseGimmick;

UCLASS(BlueprintType)
class CR4S_API UItemGimmickSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

#pragma region UGameInstanceSubsystem Override

public:
	UItemGimmickSubsystem();

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;

#pragma endregion

#pragma region DataTable Find

public:
	TArray<FName> GetItemDataRowNames() const;
	const FItemInfoData* FindItemInfoData(const FName& RowName) const;
	const FGimmickInfoData* FindGimmickInfoData(const FName& RowName) const;

	UFUNCTION(BlueprintCallable, Category = "ItemGimmickSubsystem|Find Data")
	void GetItemInfoData(const FName& RowName, FItemInfoData& OutItemInfoData) const;
	UFUNCTION(BlueprintCallable, Category = "ItemGimmickSubsystem|Find Data")
	void GetGimmickInfoData(const FName& RowName, FGimmickInfoData& OutGimmickInfoData) const;

private:
	template <typename RowStruct>
	const RowStruct* FindDataFromDataTable(const TSoftObjectPtr<UDataTable>& DataTable, const FName& RowName,
	                                       const FString& Context) const
	{
		if (DataTable.IsValid() && IsValid(DataTable.Get()))
		{
			if (const RowStruct* Row = DataTable->FindRow<RowStruct>(RowName, Context))
			{
				return Row;
			}
		}

		return nullptr;
	}

	UPROPERTY()
	TObjectPtr<UDataTable> ItemInfoDataTable;
	UPROPERTY()
	TObjectPtr<UDataTable> GimmickInfoDataTable;

#pragma endregion

#pragma region Create Inventory Item

public:
	static UBaseInventoryItem* CreateInventoryItem(UObject* Outer, const FGameplayTagContainer& ItemTags);
	
#pragma endregion 
	
#pragma region Gimmick Spawn

public:
	template <typename GimmickClass>
	GimmickClass* SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation,
	                                    const FRotator& SpawnRotation)
	{
		ABaseGimmick* BaseGimmick = SpawnGimmick(RowName, SpawnLocation, SpawnRotation);
		GimmickClass* Gimmick = Cast<GimmickClass>(BaseGimmick);

		return Gimmick;
	}

	UFUNCTION(BlueprintCallable, Category = "ItemGimmickSubsystem|Gimmick Spawn")
	void SpawnItemPouch(const AActor* SourceActor, const TMap<FName, int32>& RemainingItems,
	                    float ForwardOffset = 50.f);

private:
	UFUNCTION(BlueprintCallable, Category = "ItemGimmickSubsystem|Gimmick Spawn")
	ABaseGimmick* SpawnGimmick(const FName& RowName, const FVector& SpawnLocation, const FRotator& SpawnRotation) const;

	UPROPERTY()
	TObjectPtr<UProjectilePoolSubsystem> PoolSubsystem;

#pragma endregion

#pragma region Save & Load

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|SaveGame")
	FGimmickSaveGame GetGimmickSaveGame() const;
	UFUNCTION(BlueprintCallable, Category = "Gimmick|LoadGame")
	void LoadGimmickSaveGame(const FGimmickSaveGame& GimmickSaveGame);

	UFUNCTION(BlueprintCallable, Category = "Building|SaveGame")
	USaveGame* GetBuildingSaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "Building|LoadGame")
	void LoadBuildingSaveGame(USaveGame* NewBuildingSaveGame) const;

	UFUNCTION(BlueprintCallable, Category = "Building|LoadGame")
	FORCEINLINE void SetBuildingSaveGame(USaveGame* NewBuildingSaveGame) { BuildingSaveGame = NewBuildingSaveGame; }

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateBuildingSaveGame);

	UPROPERTY(BlueprintAssignable, Category = "Building|SaveGame")
	FOnCreateBuildingSaveGame OnCreateBuildingSaveGame;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadBuildingSaveGame, USaveGame*, BuildingSaveGame);

	UPROPERTY(BlueprintAssignable, Category = "Building|SaveGame")
	FOnLoadBuildingSaveGame OnLoadBuildingSaveGame;

private:
	UPROPERTY()
	TObjectPtr<USaveGame> BuildingSaveGame;

#pragma endregion
};
