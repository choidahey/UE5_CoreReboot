#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"

#include "TreeGimmick.generated.h"

class UWorldTimeManager;
class UDestructibleComponent;

UCLASS(BlueprintType)
class CR4S_API ATreeGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleGimmick Override

public:
	ATreeGimmick();

	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region UDestructibleComponent

protected:
	virtual void OnGimmickDestroy(AActor* DamageCauser) override;

#pragma endregion

#pragma region Destroy

public:
	UFUNCTION(BlueprintCallable, Category = "TreeGimmick|Destroy")
	void RemoveTrunk() const;
	UFUNCTION(BlueprintCallable, Category = "TreeGimmick|Respawn")
	void RespawnTrunk();
	
	UFUNCTION(BlueprintPure, Category = "TreeGimmick|Destroy")
	FORCEINLINE bool IsTrunkDestroyed() const { return bIsTrunkDestroyed; }
	UFUNCTION(BlueprintCallable, Category = "TreeGimmick|Destroy")
	FORCEINLINE void SetIsTrunkDestroyed(const bool bNewIsTrunkDestroyed) { bIsTrunkDestroyed = bNewIsTrunkDestroyed; }
	
	void HandleDestroyTrunk(const AActor* DamageCauser);
	
private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> TrunkMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Destroy")
	bool bIsTrunkDestroyed;
	
	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	float StumpHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	float ImpulseStrength;

	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	float RemoveTrunkDelay;

	FTransform OriginTrunkTransform;
	
	FTimerHandle RemoveTrunkTimerHandle;
	
	float TrunkHealth;

#pragma endregion
	
};
