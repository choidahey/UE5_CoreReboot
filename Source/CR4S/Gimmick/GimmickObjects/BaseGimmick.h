#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGimmick.generated.h"

class UItemGimmickSubsystem;
struct FGimmickInfoData;
struct FItemInfoData;
struct FResourceItemData;

UCLASS()
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

#pragma endregion

#pragma region GimmickData

public:
	const FGimmickInfoData* GetGimmickInfoData() const;
	
	UFUNCTION(BlueprintPure, Category = "Gimmick|Data")
	FORCEINLINE FName GetGimmickDataRowName() const { return GimmickData.RowName; }
	FORCEINLINE void SetGimmickDataRowName(const FName RowName) { GimmickData.RowName = RowName; }

protected:
	virtual void GetResources(const AActor* InventoryOwnerActor) const;
	virtual void GimmickDestroy();

	UPROPERTY()
	TObjectPtr<UItemGimmickSubsystem> ItemGimmickSubsystem;
	
private:
	UPROPERTY(EditAnywhere, Category = "GimmickData")
	FDataTableRowHandle GimmickData;

#pragma endregion
};
