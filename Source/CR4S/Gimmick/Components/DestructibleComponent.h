#pragma once

#include "CoreMinimal.h"
#include "DestructibleInterface.h"
#include "Components/ActorComponent.h"
#include "DestructibleComponent.generated.h"

DECLARE_DELEGATE(FOnDestry);
DECLARE_DELEGATE_TwoParams(FOnTakeDamage, float, float);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UDestructibleComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UDestructibleComponent();
	
#pragma endregion

#pragma region Initialize

public:
	void InitComponent(const float InMaxHealth);
	
#pragma endregion
	
#pragma region Health

public:
	UFUNCTION(BlueprintCallable, Category = "DestructibleComponent|Health")
	void TakeDamage(AController* DamageCauserController, float DamageAmount);

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
private:
	UPROPERTY(VisibleAnywhere, Category = "DestructibleComponent|Health")
	float MaxHealth;
	UPROPERTY(VisibleAnywhere, Category = "DestructibleComponent|Health")
	float CurrentHealth;

#pragma endregion

#pragma region Delegate

public:
	FOnTakeDamage OnTakeDamage;
	FOnDestry OnDestroy;
	
#pragma endregion
};
