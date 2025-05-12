#pragma once

#include "CoreMinimal.h"
#include "DestructibleInterface.h"
#include "Components/ActorComponent.h"
#include "DestructibleComponent.generated.h"

DECLARE_DELEGATE(FOnDestry);

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UDestructibleComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UDestructibleComponent();
	
#pragma endregion

#pragma region UDestructibleComponent Health

public:
	UFUNCTION(BlueprintCallable, Category = "DestructibleComponent|Health")
	void TakeDamage(AController* DamageCauserController, float DamageAmount);

	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "DestructibleComponent|Health")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, Category = "DestructibleComponent|Health")
	float CurrentHealth;

#pragma endregion

#pragma region Delegate

public:
	FOnDestry OnDestroy;
	
#pragma endregion
};
