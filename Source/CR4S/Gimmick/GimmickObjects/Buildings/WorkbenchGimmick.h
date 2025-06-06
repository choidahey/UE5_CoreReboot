#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "WorkbenchGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API AWorkbenchGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleResourceGimmick Override

public:
	AWorkbenchGimmick();

	virtual void BeginPlay() override;
	
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

#pragma region Crafting

public:
	FORCEINLINE int32 GetCraftingDifficulty() const { return CraftingDifficulty; }
	FORCEINLINE void SetCraftingDifficulty(const int32 NewCraftingDifficulty)
	{
		CraftingDifficulty = NewCraftingDifficulty;
	}

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crafting", meta = (ClampMin = "1", ClampMax = "3"))
	int32 CraftingDifficulty;

#pragma endregion
};
