#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "ResourcePickupGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API AResourcePickupGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	AResourcePickupGimmick();

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
};
