#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MonsterAggroComponent.generated.h"

struct FAggroInfo
{
	float RawDamageAggro = 0.f;
	float LastDistance    = 0.f;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UMonsterAggroComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMonsterAggroComponent();

	void AddDamageAggro(AActor* Instigator, float Damage);
	AActor* GetCurrentAggroTarget() const { return CurrentTarget.Get(); }
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ApplyToBlackboard(class UBlackboardComponent* BB, const struct FBlackboardKeySelector& KeySelector) const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float DamageFactor = 0.05f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float DistanceExponent = 2.f;
	
	UPROPERTY(EditAnywhere, Category="Aggro")
	float SwitchHysteresis = 0.2f;

private:
	void RecalculateAggro();
	
	TMap<TObjectPtr<AActor>, FAggroInfo> AggroTable;
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
};
