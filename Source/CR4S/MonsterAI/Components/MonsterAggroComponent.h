#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterAggroComponent.generated.h"

struct FAggroInfo
{
	float RawDamageAggro = 0.f;
	float LastDistance = 0.f;
	float LastDamageTime = 0.f;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UMonsterAggroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterAggroComponent();

	void AddDamageAggro(AActor* Instigator, float Damage);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	AActor* GetCurrentAggroTarget() const { return CurrentTarget.Get(); }

	UFUNCTION(BlueprintCallable)
	bool HasAggroFor(AActor* Target) const;

	UFUNCTION(BlueprintCallable)
	bool HasAnyAggro() const;

	UFUNCTION(BlueprintCallable)
	void CleanupExpiredAggro();

	UFUNCTION(BlueprintCallable)
	float GetAggroValueFor(AActor* Target) const;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float DamageFactor = 0.1f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float DistanceExponent = 2.f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float SwitchHysteresis = 0.2f;

	UPROPERTY(EditAnywhere, Category="Aggro")
	float AggroDecayRate = 50.0f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float AggroDecayDelay = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float MinAggroThreshold = 0.1f;
	
private:
	void RecalculateAggro();
	void DecayAggro(float DeltaTime);
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
	TMap<TObjectPtr<AActor>, FAggroInfo> AggroTable;
};
