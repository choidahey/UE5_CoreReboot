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

	UPROPERTY()
	TObjectPtr<UDataTable> ItemDataTable;
	UPROPERTY()
	TObjectPtr<UDataTable> GimmickDataTable;
	
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
	ABaseGimmick* SpawnGimmick(const FName& RowName, const FVector& SpawnLocation) const;

#pragma endregion
};
