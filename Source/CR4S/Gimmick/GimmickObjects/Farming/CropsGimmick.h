#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "CropsGimmick.generated.h"

class UInteractableComponent;

UCLASS(BlueprintType)
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

private:
	UFUNCTION()
	virtual void OnGimmickInteracted(AActor* Interactor);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
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

#pragma region Farming

public:
	void Harvest(const AActor* Interactor);

private:
	UPROPERTY(EditAnywhere, Category = "Grow")
	bool bIsHarvestable;

#pragma endregion

#pragma region Grow

public:
	FORCEINLINE float GetCurrentGrowthPercent() const { return CurrentGrowthPercent; }

private:
	UFUNCTION()
	void Grow(int64 NewPlayTime);
	void UpdateGrowthStage();

	void BindDelegate();
	void UnBindDelegate();

	UPROPERTY(VisibleAnywhere, Category = "Grow")
	int32 GrowthTimeMinutes;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float ElapsedSeconds;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float TotalGrowthSeconds;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	int32 MaxStageCount;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	int32 StageDuration;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	int32 CurrentStage;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	float CurrentGrowthPercent;

	UPROPERTY(VisibleAnywhere, Category = "Grow")
	TArray<TObjectPtr<UStaticMesh>> CropsMeshes;

	int64 PrevPlayTime;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGrow, const float, GrowthPercent);

	UPROPERTY(BlueprintAssignable, Category = "CropsGimmick|Delegates")
	FOnGrow OnGrow;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHarvest);

	UPROPERTY(BlueprintAssignable, Category = "CropsGimmick|Delegates")
	FOnHarvest OnHarvest;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCropComposted);

	UPROPERTY(BlueprintAssignable, Category = "CropsGimmick|Delegates")
	FOnCropComposted OnCropComposted;

#pragma endregion
};
