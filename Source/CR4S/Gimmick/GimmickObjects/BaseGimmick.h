#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGimmick.generated.h"

struct FGimmickSaveGameData;
class UObjectPoolComponent;
class UItemGimmickSubsystem;
struct FGimmickInfoData;
struct FItemInfoData;
struct FResourceItemData;

UCLASS(BlueprintType)
class CR4S_API ABaseGimmick : public AActor
{
	GENERATED_BODY()

#pragma region AActor Override

public:
	ABaseGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Components

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UStaticMeshComponent* GetGimmickMeshComponent() const { return GimmickMeshComponent; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> GimmickMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UObjectPoolComponent> ObjectPoolComponent;

#pragma endregion

#pragma region GimmickData

public:
	const FGimmickInfoData* GetGimmickInfoData() const;
	
	UFUNCTION(BlueprintPure, Category = "Gimmick|Data")
	FORCEINLINE FName GetGimmickDataRowName() const { return GimmickData.RowName; }
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Data")
	FORCEINLINE void SetGimmickDataRowName(const FName RowName) { GimmickData.RowName = RowName; }
	UFUNCTION(BlueprintCallable, Category = "Gimmick|Destroy")
	virtual void GimmickDestroy();

protected:
	virtual void GetResources(const AActor* InventoryOwnerActor) const;

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;
	
private:
	UPROPERTY(EditAnywhere, Category = "GimmickData")
	FDataTableRowHandle GimmickData;

#pragma endregion

#pragma region Save & Load

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gimmick|LoadData")
	FGimmickSaveGameData GetGimmickSaveGameData(bool& bSuccess);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Gimmick|LoadData")
	void LoadGimmickSaveGameData(const FGimmickSaveGameData& GimmickSaveGameData);
	
#pragma endregion 
};
