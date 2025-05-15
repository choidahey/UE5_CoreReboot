#pragma once

#include "CoreMinimal.h"
#include "CR4S/Gimmick/GimmickObjects/BaseGimmick/BaseGimmick.h"
#include "DestructibleGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API ADestructibleGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	ADestructibleGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UDestructibleComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

protected:
	/** IMPLEMENTED IN THE INHERITED CLASS */
	UFUNCTION()
	virtual void OnGimmickTakeDamage(float DamageAmount, float CurrentHealth);
	/** IMPLEMENTED IN THE INHERITED CLASS */
	UFUNCTION()
	virtual void OnGimmickDestroy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;

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
};
