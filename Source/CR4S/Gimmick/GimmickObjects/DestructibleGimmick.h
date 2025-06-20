#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

#include "DestructibleGimmick.generated.h"

class ABaseDestructObject;
class UDestructibleComponent;

UCLASS(BlueprintType)
class CR4S_API ADestructibleGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override
	
public:
	ADestructibleGimmick();

	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
#pragma endregion
	
#pragma region UDestructibleComponent

public:
	FORCEINLINE void SetDestroyDelay(const float NewDelay) { DestroyDelay = NewDelay; }
	
protected:
	UFUNCTION()
	virtual void OnGimmickTakeDamage(AActor* DamageCauser, float DamageAmount, float CurrentHealth);
	UFUNCTION()
	virtual void OnGimmickDestroy(AActor* DamageCauser);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDestructibleComponent> DestructibleComponent;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	TSubclassOf<ABaseDestructObject> DestructObjectClass;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;
	
#pragma endregion

#pragma region BonusDamageTag

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BonusDamage")
	FGameplayTag ToolBonusDamageTag;

	UPROPERTY(EditDefaultsOnly, Category = "BonusDamage")
	float ToolBonusDamageMultiplier;
	
#pragma endregion
	
#pragma region Shake

private:
	void StartShake();
	void PerformShake();
	void StopShake();

	FTimerHandle ShakeTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Shake")
	bool bCanShake;
	UPROPERTY(EditDefaultsOnly, Category = "Shake")
	float ShakeDuration;
	UPROPERTY(EditDefaultsOnly, Category = "Shake")
	float ShakeInterval;
	UPROPERTY(EditDefaultsOnly, Category = "Shake")
	float ShakeIntensity;

	UPROPERTY(VisibleAnywhere, Category = "Shake")
	FVector OriginalLocation;
	UPROPERTY(VisibleAnywhere, Category = "Shake")
	float ElapsedTime;
	
#pragma endregion
	
};
