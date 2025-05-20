#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/BaseDataInfo.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ItemGimmickSubsystem.generated.h"


UCLASS()
class CR4S_API UItemGimmickSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region UGameInstanceSubsystem Override

public:
	UItemGimmickSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
#pragma endregion

#pragma region DataTable Load

public:
	UFUNCTION(BlueprintCallable, Category = "ItemGimmickSubsystem|Load")
	void LoadDataTableAsync();

	UTexture2D* LoadIcon(const TSoftObjectPtr<UTexture2D>& IconRef);
	
private:
	void OnDataTableLoaded(TSoftObjectPtr<UDataTable>& DataTable, bool& bIsDataTableLoaded, const FString& DataTableName) const;
	
	void OnItemDataTableLoaded();
	void OnGimmickDataTableLoaded();
	
	void CheckAllDataTableLoaded() const;
	
	void UnloadAllDataTables();

	FSoftObjectPath DefaultItemDataTablePath;
	FSoftObjectPath DefaultGimmickDataTablePath;
	
	UPROPERTY()
	TSoftObjectPtr<UDataTable> ItemDataTable;
	UPROPERTY()
	TSoftObjectPtr<UDataTable> GimmickDataTable;
	
	bool bIsItemDataTableLoaded;
	bool bIsGimmickDataTableLoaded;

	TSet<FSoftObjectPath> LoadedPaths;
	
#pragma endregion

#pragma region DataTable Find

public:
	TArray<FName> GetItemDataRowNames() const;
	const FBaseItemData* FindItemData(const FName& RowName) const;
	const FBaseGimmickData* FindGimmickData(const FName& RowName) const;
	
private:
	template<typename RowType>
	const RowType* FindRowFromDataTable(const TSoftObjectPtr<UDataTable>& DataTable, const FName& RowName, const FString& Context) const
	{
		if (DataTable.IsValid() && IsValid(DataTable.Get()))
		{
			if (const RowType* Row = DataTable->FindRow<RowType>(RowName, Context))
			{
				return Row;
			}
		}
		
		return nullptr;
	}
	
#pragma endregion
	
#pragma region Gimmick Spawn

public:
	template<typename GimmickClass>
	GimmickClass* SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation)
	{
		ABaseGimmick* BaseGimmick = SpawnGimmick(RowName, SpawnLocation);
		GimmickClass* Gimmick = Cast<GimmickClass>(BaseGimmick);

		return Gimmick;
	}

private:
	ABaseGimmick* SpawnGimmick(const FName& RowName, const FVector& SpawnLocation);

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDataTablesReady);
	FOnDataTablesReady OnDataTablesReady;
	
#pragma endregion
};
