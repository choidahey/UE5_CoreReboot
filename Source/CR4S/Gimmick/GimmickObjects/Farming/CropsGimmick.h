#pragma once

#include "CoreMinimal.h"
#include "Gimmick/Data/GimmickData.h"
#include "Gimmick/GimmickObjects/BaseGimmick.h"
#include "CropsGimmick.generated.h"

class AEnvironmentManager;
class UEnvironmentalStatusComponent;
class UInteractableComponent;

USTRUCT(BlueprintType)
struct FCropsGimmickGrowthData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Growth")
	int32 GrowthTimeMinutes = 0;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	float ElapsedSeconds = 0.f;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	float TotalGrowthSeconds = 0.f;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	int32 MaxStageCount = 0;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	int32 StageDuration = 0;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	int32 CurrentStage = 0;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	float CurrentGrowthPercent = 0.f;
	UPROPERTY(VisibleAnywhere, Category = "Growth")
	int64 PrevPlayTime = -1;
	UPROPERTY()
	FCropsGimmickData CropsGimmickData;
};

UCLASS(BlueprintType)
class CR4S_API ACropsGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	ACropsGimmick();

	virtual void BeginPlay() override;

	virtual FGimmickSaveGameData GetGimmickSaveGameData_Implementation(bool& bSuccess) override;

private:
	void InitCollisionSetting() const;
	
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

	UFUNCTION(BlueprintPure, Category = "CropsGimmick|Farming")
	FORCEINLINE bool IsPlanted() const { return bIsPlanted; }

private:
	UPROPERTY(EditAnywhere, Category = "Grow")
	bool bIsHarvestable;
	UPROPERTY(VisibleAnywhere, Category = "Grow")
	bool bIsPlanted;

#pragma endregion

#pragma region Growth

public:
	UFUNCTION(BlueprintCallable, Category = "Growth")
	void OnPlant();

	FORCEINLINE float GetCurrentGrowthPercent() const { return CropsGimmickGrowthData.CurrentGrowthPercent; }
	FORCEINLINE bool GetIsHarvestable() const { return bIsHarvestable; }

private:
	void InitGrowthState();

	float GetCalculatedSeconds();
	
	UFUNCTION()
	void Grow(int64 NewPlayTime);
	void UpdateGrowthStage();

	void BindDelegate();
	void UnBindDelegate();

	UPROPERTY(VisibleAnywhere, Category = "Growth")
	TArray<TObjectPtr<UStaticMesh>> CropsMeshes;

	FCropsGimmickGrowthData CropsGimmickGrowthData;

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

#pragma region EnvironmentalStatus

private:
	UFUNCTION()
	void HandleTemperatureBreach(int32 BreachCode);
	UFUNCTION()
	void HandleHumidityBreach(int32 BreachCode);
	UFUNCTION()
	void HandleTemperatureNormalized();
	UFUNCTION()
	void HandleHumidityNormalized();

	void InitEnvironmentalStatus() const;
	void BindEnvStatusDelegate();
	void CheckIsDay(bool& bSuccess, bool& bIsDay) const;
	void CheckAcceleration();

	
	UPROPERTY(VisibleDefaultsOnly, Category = "EnvironmentalStatus|Components")
	TObjectPtr<UEnvironmentalStatusComponent> EnvironmentalStatus;
	UPROPERTY()
	TObjectPtr<AEnvironmentManager> EnvironmentManager;

	UPROPERTY(VisibleAnywhere, Category = "EnvironmentalStatus")
	float HeatSlowdownMultiplier;
	UPROPERTY(VisibleAnywhere, Category = "EnvironmentalStatus")
	float HumiditySlowdownMultiplier;
	UPROPERTY(VisibleAnywhere, Category = "EnvironmentalStatus")
	float AccelerationMultiplier;
	
#pragma endregion

#pragma region Sound

private:
	UPROPERTY()
	TObjectPtr<USoundBase> HarvestSound; 
	
#pragma endregion 

#pragma region Save & Load

public:
	UFUNCTION(BlueprintPure, Category = "CropsGimmick|SaveGame")
	FORCEINLINE FCropsGimmickGrowthData GetCropsGimmickData() const { return CropsGimmickGrowthData; }

	UFUNCTION(BlueprintCallable, Category = "CropsGimmick|LoadGame")
	void LoadPlantedCropsGimmick(const FCropsGimmickGrowthData& NewCropsGimmickData);

#pragma endregion
};
