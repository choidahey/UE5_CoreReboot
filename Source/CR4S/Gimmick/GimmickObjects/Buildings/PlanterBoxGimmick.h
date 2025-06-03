#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "Inventory/InventoryItem/BaseInventoryItem.h"
#include "PlanterBoxGimmick.generated.h"

class UPlanterBoxInventoryComponent;
class ACropsGimmick;
class UInteractableComponent;

UCLASS()
class CR4S_API APlanterBoxGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleResourceGimmick Override

public:
	APlanterBoxGimmick();

	virtual void BeginPlay() override;

	virtual void OnGimmickDestroy(AActor* DamageCauser) override;

#pragma endregion
	
#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	UFUNCTION()
	virtual void OnGimmickInteracted(AActor* Interactor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;

private:
	UFUNCTION()
	void OnDetectionStateChanged(AActor* InDetectingActor, bool bInIsDetected);

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	AActor* DetectingActor;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	bool bIsDetected;

#pragma endregion

#pragma region Farming

public:
	FORCEINLINE ACropsGimmick* GetPlantedGimmick() const { return PlantedGimmick; }
	
private:
	UFUNCTION()
	void HandlePlantingCropsGimmick(UBaseInventoryItem* Item);
	
	UFUNCTION()
	void HandleHarvest();
	
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<USceneComponent> SpawnPoint;

	UPROPERTY()
	TObjectPtr<ACropsGimmick> PlantedGimmick;
	
#pragma endregion

#pragma region Inventory
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UPlanterBoxInventoryComponent> PlanterBoxInventoryComponent;
	
#pragma endregion
	
};
