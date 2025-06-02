#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"

#include "TreeGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
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
	void HandleDestroyTrunk(const AActor* DamageCauser);
	
private:
	void RemoveTrunk() const;
	
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
	

#pragma endregion
	
};
