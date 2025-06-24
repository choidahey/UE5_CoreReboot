#pragma once

#include "CoreMinimal.h"

#include "ConsumableItemSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FFreshnessInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsRotten = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShelfLifeSeconds = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RemainingFreshnessTime = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecayRateMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
