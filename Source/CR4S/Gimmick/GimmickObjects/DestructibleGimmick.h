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
	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	bool bIsActorDestroyOnDestroyAction;

	FTimerHandle DestroyTimerHandle;
	
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyDelay;

#pragma endregion
	
};
