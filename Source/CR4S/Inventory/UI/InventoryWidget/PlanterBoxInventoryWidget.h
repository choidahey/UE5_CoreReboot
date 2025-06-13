#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryWidget.h"
#include "PlanterBoxInventoryWidget.generated.h"

class ACropsGimmick;

UCLASS()
class CR4S_API UPlanterBoxInventoryWidget : public UBaseInventoryWidget
{
	GENERATED_BODY()

#pragma region UBaseInventoryWidget Override

public:
	UPlanterBoxInventoryWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void ConnectInventoryComponent(UBaseInventoryComponent* NewInventoryComponent) override;

	virtual void UpdateItemSlotWidget(int32 SlotIndex, UBaseInventoryItem* Item) override;
	virtual void UnBoundWidgetDelegate() override;
	
#pragma endregion

#pragma region UPlanterBoxInventoryWidget

public:
	UFUNCTION(BlueprintCallable, Category = "PlanterBoxInventoryWidget")
	FORCEINLINE void ConnectPlantedCropsGimmick(ACropsGimmick* NewPlantedCropsGimmick)
	{
		PlantedCropsGimmick = NewPlantedCropsGimmick;
	}
	
	UFUNCTION()
	void UpdatePlanterBoxInfo(float CurrentGrowthPercent);

	UFUNCTION()
	void ClearPlanterBoxInfo();

private:
	void ConnectCropsGimmick();
	
	UPROPERTY(EditDefaultsOnly, Category = "PlanterBoxInfo")
	FText DefaultNameText;
	UPROPERTY(EditDefaultsOnly, Category = "PlanterBoxInfo", meta = (MultiLine = true))
	FText DefaultInfoText;

	UPROPERTY(EditDefaultsOnly, Category = "PlanterBoxInfo")
	FText GrowthPercentText;

	UPROPERTY()
	TObjectPtr<ACropsGimmick> PlantedCropsGimmick;

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CropsNameTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlanterBoxInfoTextBlock;

#pragma endregion
};
