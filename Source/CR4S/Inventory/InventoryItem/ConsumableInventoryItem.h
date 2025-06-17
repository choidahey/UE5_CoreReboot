#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "ConsumableInventoryItem.generated.h"

class UWorldTimeManager;

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
	int32  ResistanceValue = 0;
	int64  PrevPlayTime = -1;
};

UCLASS()
class CR4S_API UConsumableInventoryItem : public UBaseInventoryItem
{
	GENERATED_BODY()

#pragma region UBaseInventoryItem Override
	
public:
	UConsumableInventoryItem();
	
	virtual void InitInventoryItem(UBaseInventoryComponent* NewInventoryComponent, const FInventoryItemData& NewInventoryItemData, const int32 StackCount = 0) override;
	
	virtual void UseItem(int32 Index) override;
	virtual void HandlePassiveEffect(int64 NewPlayTime) override;

#pragma endregion

#pragma region Data
public:
	FORCEINLINE const FConsumableItemData& GetConsumableItemData() const { return ConsumableItemData; }
	
private:
	void ApplyResistanceEffect();

	void AddResistanceBuff(EResistanceBuffType Type, int32 Value, float DurationMinutes);
	
	UFUNCTION()
	void UpdateAllResistanceEffects(int64 NewPlayTime);
	void ClearAllEffects();
	void ClearResistanceEffect(EResistanceBuffType Type);
	
	void ApplyThreshold(EResistanceBuffType Type, int32 Value) const;
	
	FConsumableItemData ConsumableItemData;
	
	TMap<EResistanceBuffType, FResistanceEffect> ResistanceEffects;

	float Freshness;
	float DecayRateMultiplier;
	

#pragma endregion
};
