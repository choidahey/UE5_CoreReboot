#pragma once

#include "CoreMinimal.h"
#include "BaseGimmick.h"
#include "ItemPouchGimmick.generated.h"

class UBaseInventoryItem;
class UBaseInventoryComponent;
class UInteractableComponent;

UCLASS()
class CR4S_API AItemPouchGimmick : public ABaseGimmick
{
	GENERATED_BODY()
	
#pragma region ABaseGimmick Override

public:
	AItemPouchGimmick();

	virtual void BeginPlay() override;

#pragma endregion
	
#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

private:
	UFUNCTION()
	virtual void OnGimmickInteracted(AActor* Interactor);
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

#pragma endregion

#pragma region Initialize

public:
	void InitItemPouch(const TArray<UBaseInventoryItem*>& Items) const;
	
#pragma endregion
	
#pragma region Inventory
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;
	
#pragma endregion
};
