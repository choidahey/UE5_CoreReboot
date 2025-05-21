#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "GrowableGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API AGrowableGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override
	
public:
	AGrowableGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	UFUNCTION()
	virtual void OnGimmickInteracted(AController* Controller);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;
	
private:
	UFUNCTION()
	void OnDetectionStateChanged(AController* InDetectingController, bool bInIsDetected);
	void UpdateInteractionText() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText HarvestText;

	FText DefaultInteractionText;
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	AController* DetectingController;
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	bool bIsDetected;
	

#pragma endregion
	
#pragma region Grow

public:
	FORCEINLINE float GetCurrentGrowthPercent() const { return CurrentGrowthPercent; }

protected:
	virtual void GrowthStageChanged(const int32 NewGrowthStage);
	
private:
	bool IsHeldItemSeed() const;
	void Grow();
	int32 CalculateGrowthStage() const;
	
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	bool bIsHarvestable;

	UPROPERTY(VisibleAnywhere, Category = "Farming")
	float GrowthPercentPerInterval;
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	float IntervalSeconds;
	UPROPERTY(EditDefaultsOnly, Category = "Farming")
	TArray<float> GrowthStageThresholds;
	UPROPERTY(EditDefaultsOnly, Category = "Farming")
	TArray<float> GrowthStageScale;
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	int32 PreviousGrowthStage;
	
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	float CurrentGrowthPercent;
	
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	float MaxGrowthPercent;
	
	FTimerHandle GrowthTimerHandle;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DELEGATE(FOnHarvest)
	FOnHarvest OnHarvest;
	
#pragma endregion
};
