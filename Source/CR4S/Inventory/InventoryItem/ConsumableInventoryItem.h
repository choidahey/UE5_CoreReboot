#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "Inventory/Data/ConsumableItemSaveGame.h"
#include "ConsumableInventoryItem.generated.h"

class UWorldTimeManager;

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
	FORCEINLINE bool IsRotten() const { return FreshnessInfo.bIsRotten; }
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
		FreshnessInfo.bIsRotten = NewFreshnessInfo.bIsRotten;
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

#pragma region Save & Load

public:
	virtual FInventoryItemSaveGame GetInventoryItemSaveData() override;
	virtual void LoadInventoryItemSaveData(UBaseInventoryComponent* NewInventoryComponent, const FInventoryItemSaveGame& ItemSaveGame) override;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFreshnessChanged, const float, FreshnessPercent);

	FOnFreshnessChanged OnFreshnessChanged;

#pragma endregion
};
