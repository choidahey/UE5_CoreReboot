#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

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

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
#pragma endregion
	
#pragma region UDestructibleComponent

public:
	UFUNCTION(BlueprintPure, Category = "ADestructibleResourceGimmick|Components")
	FORCEINLINE UDestructibleComponent* GetDestructibleComponent() const { return DestructibleComponent; }

	FORCEINLINE void SetDestroyDelay(const float NewDelay) { DestroyDelay = NewDelay; }

protected:
	UFUNCTION()
	virtual void OnGimmickTakeDamage(AActor* DamageCauser, float DamageAmount, float CurrentHealth);
	UFUNCTION()
	virtual void OnGimmickDestroy(AActor* DamageCauser);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UDestructibleComponent> DestructibleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;
	
private:
	UPROPERTY(EditAnywhere, Category = "Destroy")
	TEnumAsByte<ECollisionChannel> DestructibleGimmickCollisionChannel;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyImpulseRadius;
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyImpulseStrength;
	
	FTimerHandle DestroyTimerHandle;
	
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
