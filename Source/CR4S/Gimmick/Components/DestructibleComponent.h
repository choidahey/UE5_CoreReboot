#pragma once

#include "CoreMinimal.h"
#include "DestructibleInterface.h"
#include "Components/ActorComponent.h"
#include "DestructibleComponent.generated.h"

USTRUCT(BlueprintType)
struct FDestructibleDamageData
{
	GENERATED_BODY()

public:
	FDestructibleDamageData()
		: Instigator(nullptr)
		  , DamageAmount(0)
		  , HitLocation()
		  , ImpulseDir()
		  , ImpulseStrength(0)
	{
	}

	FDestructibleDamageData(AController* InInstigator,
	                        const float InDamageAmount,
	                        const FVector& InHitLocation,
	                        const FVector& InImpulseDir,
	                        const float InImpulseStrength)
		: Instigator(InInstigator)
		  , DamageAmount(InDamageAmount)
		  , HitLocation(InHitLocation)
		  , ImpulseDir(InImpulseDir)
		  , ImpulseStrength(InImpulseStrength)
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DestructibleDamageData")
	TObjectPtr<AController> Instigator;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DestructibleDamageData")
	float DamageAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DestructibleDamageData")
	FVector HitLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DestructibleDamageData")
	FVector ImpulseDir;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "DestructibleDamageData")
	float ImpulseStrength;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UDestructibleComponent : public UActorComponent, public IDestructibleInterface
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UDestructibleComponent();

#pragma endregion

#pragma region IDestructibleInterface Implement

	virtual void ApplyDamage(float DamageAmount, const FVector& HitLocation, const FVector& ImpulseDir,
	                         float ImpulseStrength) override;

	virtual void ApplyRadiusDamage(float BaseDamage, const FVector& HurtOrigin, float DamageRadius,
	                               float ImpulseStrength, bool bFullDamage) override;

#pragma endregion

#pragma region UDestructibleComponent Health

public:
	UFUNCTION(BlueprintCallable, Category = "DestructibleComponent|Health")
	void TakeDamage(const FDestructibleDamageData& DestructibleDamageData);

	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "DestructibleComponent|Health")
	float CurrentHealth;
	UPROPERTY(EditDefaultsOnly, Category = "DestructibleComponent|Health")
	float MaxHealth;

#pragma endregion
};
