#pragma once

#include "CoreMinimal.h"
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

#pragma region DataTable

public:
	UFUNCTION(BlueprintCallable, Category = "DataTable")
	void LoadDataTableAsync();
	UFUNCTION(BlueprintCallable, Category = "DataTable")
	void SpawnGimmickByRowName(const FName& RowName, const FVector& SpawnLocation);
	
private:
	void OnItemDataTableLoaded();
	void OnGimmickDataTableLoaded();

	void CheckAllDataTableLoaded();
	
	UPROPERTY()
	TSoftObjectPtr<UDataTable> ItemDataTable;
	UPROPERTY()
	TSoftObjectPtr<UDataTable> GimmickDataTable;

	bool bIsItemDataTableLoaded;
	bool bIsGimmickDataTableLoaded;
	bool bIsAllDataTableLoaded;

#pragma endregion
};
