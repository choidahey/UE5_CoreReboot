#pragma once

#include "CoreMinimal.h"

#include "HealthDecayData.generated.h"

USTRUCT(BlueprintType)
struct FHealthDecayData
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	bool bIsActive = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	float MaxHealth = 7000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	float CurrentHealth = 7000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay", meta=(ClampMin="0.0", ClampMax="1.0"))
	float TargetHealthRatio = 0.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay", meta=(ClampMin="1.0"))
	float DecayDuration = 2400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	int64 PreviousPlayTime = -1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	float MinHealth = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HealthDecay")
	float HealthDecayRate = 0.f;

	void ResetHealthDecay()
	{
		bIsActive = false;
		PreviousPlayTime = -1;
		CurrentHealth = MaxHealth;
		MinHealth = MaxHealth * TargetHealthRatio;
		HealthDecayRate = (MaxHealth - MinHealth) / DecayDuration;
	}
};
