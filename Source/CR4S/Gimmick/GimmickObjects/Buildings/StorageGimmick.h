#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "StorageGimmick.generated.h"

class UBaseInventoryComponent;
class UInteractableComponent;

UCLASS()
class CR4S_API AStorageGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleResourceGimmick Override

public:
	AStorageGimmick();
	
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

#pragma endregion

#pragma region Inventory
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;
	
#pragma endregion
};
