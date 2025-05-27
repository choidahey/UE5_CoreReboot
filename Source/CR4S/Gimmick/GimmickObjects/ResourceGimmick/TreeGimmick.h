#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

#include "TreeGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API ATreeGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleGimmick Override

public:
	ATreeGimmick();

	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region UDestructibleComponent

public:
	UFUNCTION(BlueprintPure, Category = "ATreeGimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

	FORCEINLINE void SetDestroyDelay(const float NewDelay) { DestroyDelay = NewDelay; }

protected:
	UFUNCTION()
	virtual void OnGimmickTakeDamage(AActor* DamageCauser, float DamageAmount, float CurrentHealth);
	UFUNCTION()
	virtual void OnGimmickDestroy(AActor* DamageCauser);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	bool bIsActorDestroyOnDestroyAction;

	FTimerHandle DestroyTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;

#pragma endregion

#pragma region Destroy
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> TrunkMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	float StumpHealth;
	
	bool bIsTrunkDestroyed;
	
#pragma endregion
	
#pragma region Shake

private:
	void StartShake();
	void PerformShake();
	void StopShake();

	FTimerHandle ShakeTimerHandle;

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
