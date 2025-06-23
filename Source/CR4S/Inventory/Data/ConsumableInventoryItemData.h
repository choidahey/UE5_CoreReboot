#pragma once

#include "CoreMinimal.h"

#include "ConsumableInventoryItemData.generated.h"

USTRUCT(BlueprintType)
struct FFreshnessInfo
{
	GENERATED_BODY()

	bool bIsRotten = false;
	float ShelfLifeSeconds = 100.f;
	float RemainingFreshnessTime = 100.f;
	float DecayRateMultiplier = 1.0f;
	int64 PreviousDecayPlayTime = -1;
	
	bool ShouldDecay() const
	{
		return ShelfLifeSeconds > 0.f;
	}

	float GetFreshnessPercent() const
	{
		return ShelfLifeSeconds > 0.f ? FMath::Clamp(RemainingFreshnessTime / ShelfLifeSeconds, 0.f, 1.f) : 1.f;
	}
};

UENUM()
enum class EResistanceBuffType : uint8
{
	Heat UMETA(DisplayName = "더위"),
	Humidity UMETA(DisplayName = "습기"),
	Cold UMETA(DisplayName = "추위")
};

USTRUCT()
struct FResistanceEffect
{
	GENERATED_BODY()

	float Duration = 0.f;
	float Elapsed = 0.f;
	int32 ResistanceValue = 0;
	int64 PrevPlayTime = -1;
};

USTRUCT(BlueprintType)
struct FConsumableInventoryItemData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFreshnessInfo FreshnessInfo = FFreshnessInfo();
	UPROPERTY()
	TMap<EResistanceBuffType, FResistanceEffect> ResistanceEffects;
};