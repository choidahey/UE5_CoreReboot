#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "CropsGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API ACropsGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override
	
public:
	ACropsGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	UFUNCTION()
	virtual void OnGimmickInteracted(AActor* Interactor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;
	
private:
	UFUNCTION()
	void OnDetectionStateChanged(AActor* InDetectingActor, bool bInIsDetected);
	void UpdateInteractionText() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText HarvestText;

	FText DefaultInteractionText;
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	AActor* DetectingActor;
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	bool bIsDetected;
	

#pragma endregion
	
#pragma region Grow

public:
	FORCEINLINE float GetCurrentGrowthPercent() const { return CurrentGrowthPercent; }

protected:
	virtual void GrowthStageChanged(const int32 NewGrowthStage);
	
private:
	void Grow();
	int32 CalculateGrowthStage() const;
	
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	bool bIsHarvestable;

	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float GrowthPercentPerInterval;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float IntervalSeconds;
	UPROPERTY(EditDefaultsOnly, Category = "Grow")
	TArray<float> GrowthStageThresholds;
	UPROPERTY(EditDefaultsOnly, Category = "Grow")
	TArray<float> GrowthStageScale;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	int32 PreviousGrowthStage;
	
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float CurrentGrowthPercent;
	
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float MaxGrowthPercent;
	
	FTimerHandle GrowthTimerHandle;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_DELEGATE(FOnHarvest);
	FOnHarvest OnHarvest;
	
#pragma endregion
};
