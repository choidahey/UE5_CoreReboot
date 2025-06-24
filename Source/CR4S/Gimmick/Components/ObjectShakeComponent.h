#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectShakeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UObjectShakeComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UObjectShakeComponent();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	
#pragma endregion

#pragma region Shake

public:
	UFUNCTION(BlueprintCallable, Category = "Shake")
	void Shake();
	UFUNCTION(BlueprintCallable, Category = "Shake")
	void StartShake();
	UFUNCTION(BlueprintCallable, Category = "Shake")
	FORCEINLINE bool CanShake() const { return bCanShake; }
	
private:
	
	UFUNCTION(BlueprintCallable, Category = "Shake")
	void PerformShake();
	UFUNCTION(BlueprintCallable, Category = "Shake")
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
