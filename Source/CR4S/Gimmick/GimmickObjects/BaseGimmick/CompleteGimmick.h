#pragma once

#include "CoreMinimal.h"
#include "DestructibleGimmick.h"
#include "CompleteGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API ACompleteGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	ACompleteGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UDestructibleComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

protected:
	/** IMPLEMENTED IN THE INHERITED CLASS */
	UFUNCTION()
	virtual void OnGimmickDestroy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	bool bIsActorDestroyOnDestroyAction;
	
#pragma endregion

#pragma region Destroy

public:
	FORCEINLINE void SetDestroyDelay(const float NewDelay) { DestroyDelay = NewDelay; }
	
private:
	void DelayedDestroy();

	FTimerHandle DestroyTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;
	
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
