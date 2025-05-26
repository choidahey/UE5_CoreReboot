#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"

#include "DestructibleResourceGimmick.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API ADestructibleResourceGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override
	
public:
	ADestructibleResourceGimmick();

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
	virtual void OnGimmickTakeDamage(float DamageAmount, float CurrentHealth);
	UFUNCTION()
	virtual void OnGimmickDestroy();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDestructibleComponent> DestructibleComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

private:
	void DelayedDestroy();
	void GetResourceItem() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	bool bIsActorDestroyOnDestroyAction;

	FTimerHandle DestroyTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;

#pragma endregion
	
};
