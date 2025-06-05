#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryItem.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "ConsumableInventoryItem.generated.h"

class UWorldTimeManager;

UENUM()
enum class EResistanceBuffType : uint8
{
	Heat UMETA(DisplayName = "더위"),
	Humidity UMETA(DisplayName = "습기"),
	Cold UMETA(DisplayName = "추위")
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
	virtual void HandlePassiveEffect() override;

#pragma endregion

#pragma region Data
public:
	FORCEINLINE const FConsumableItemData& GetConsumableItemData() const { return ConsumableItemData; }
	
private:
	void ApplyResistanceEffect();
	void ClearResistanceEffect(EResistanceBuffType Type) const;

	UFUNCTION()
	void UpdateHeatResistanceEffect(int64 PlayTime);
	UFUNCTION()
	void UpdateHumidityResistanceEffect(int64 PlayTime);
	UFUNCTION()
	void UpdateColdResistanceEffect(int64 PlayTime);

	UPROPERTY()
	TObjectPtr<UWorldTimeManager> WorldTimeManager;
	
	FConsumableItemData ConsumableItemData;

	int32 HeatEffectDuration;
	int32 HumidityEffectDuration;
	int32 ColdEffectDuration;

#pragma endregion
};
