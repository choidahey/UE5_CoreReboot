#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick/DestructibleGimmick.h"
#include "TreeGimmick.generated.h"

UCLASS()
class CR4S_API ATreeGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleGimmick Override

public:
	ATreeGimmick();

	virtual void BeginPlay() override;
	
	virtual void OnGimmickTakeDamage(float DamageAmount, float CurrentHealth) override;
	virtual void OnGimmickDestroy() override;
	
#pragma endregion

#pragma region Components

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> TrunkMeshComponent;
	
#pragma endregion

#pragma region Destroy
	
private:
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
	/** 떨림 강도 */
	UPROPERTY(EditDefaultsOnly, Category = "Shake")
	float ShakeIntensity;

	UPROPERTY(VisibleAnywhere, Category = "Shake")
	FVector OriginalLocation;
	UPROPERTY(VisibleAnywhere, Category = "Shake")
	float ElapsedTime;
	
#pragma endregion
};
