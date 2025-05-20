#pragma once

#include "CoreMinimal.h"
#include "BaseGimmick.h"
#include "InteractableGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API AInteractableGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	AInteractableGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	/** IMPLEMENTED IN THE INHERITED CLASS */
	UFUNCTION()
	virtual void OnGimmickInteracted();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;

#pragma endregion
};
