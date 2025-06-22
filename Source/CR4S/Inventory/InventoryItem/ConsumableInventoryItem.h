#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "ConsumableInventoryItem.generated.h"

class UWorldTimeManager;

USTRUCT(BlueprintType)
struct FFreshnessInfo
{
	GENERATED_BODY()
	;

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

UCLASS()
class CR4S_API UConsumableInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override

public:
	UConsumableInventoryItem();

	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent,
	                               const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;

	virtual void UpdateInventoryItem(UBaseInventoryComponent* NewInventoryComponent) override;

	virtual void UseItem(int32 Index) override;
	virtual void HandlePassiveEffect(int64 NewPlayTime) override;

#pragma endregion

#pragma region Freshness

public:
	FORCEINLINE const FFreshnessInfo& GetFreshnessInfo() const { return FreshnessInfo; }

	FORCEINLINE float GetRemainingFreshnessTime() const
	{
		return FreshnessInfo.RemainingFreshnessTime;
	}

	FORCEINLINE float GetFreshnessPercent() const
	{
		return FreshnessInfo.GetFreshnessPercent();
	}

	FORCEINLINE void UpdateFreshnessInfo(const FFreshnessInfo& NewFreshnessInfo)
	{
		FreshnessInfo.RemainingFreshnessTime = NewFreshnessInfo.RemainingFreshnessTime;
		FreshnessInfo.PreviousDecayPlayTime = NewFreshnessInfo.PreviousDecayPlayTime;
	}

	FORCEINLINE void AveragingFreshness(const float OtherRemainingFreshnessTime)
	{
		FreshnessInfo.RemainingFreshnessTime
			= (FreshnessInfo.RemainingFreshnessTime + OtherRemainingFreshnessTime) / 2;
	}

	FORCEINLINE void SetDecayRateMultiplier(const float NewDecayRateMultiplier)
	{
		FreshnessInfo.DecayRateMultiplier = NewDecayRateMultiplier;
	}

private:
	UFUNCTION()
	bool UpdateFreshnessDecay(int64 NewPlayTime);
	void OnItemRotten();
	FText CreateNewDescription() const;

	FFreshnessInfo FreshnessInfo;

	bool bIsRotten;
	FText FreshnessText;
	FText DefaultDescription;
	FText RottenDescription;

#pragma endregion

#pragma region ApplyEffect

private:
	void ApplyResistanceEffect();

	void AddResistanceBuff(EResistanceBuffType Type, int32 Value, float DurationMinutes);

	UFUNCTION()
	void UpdateAllResistanceEffects(int64 NewPlayTime);
	void ClearAllEffects();
	void ClearResistanceEffect(EResistanceBuffType Type);

	void ApplyThreshold(EResistanceBuffType Type, int32 Value) const;

	TMap<EResistanceBuffType, FResistanceEffect> ResistanceEffects;

#pragma endregion

#pragma region Data

public:
	FORCEINLINE const FConsumableItemData& GetConsumableItemData() const { return ConsumableItemData; }

private:
	FConsumableItemData ConsumableItemData;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreshnessChanged, const float, FreshnessPercent);

	FOnFreshnessChanged OnFreshnessChanged;

#pragma endregion
};
