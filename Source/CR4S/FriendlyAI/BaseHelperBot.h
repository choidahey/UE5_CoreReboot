#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HelperBotState.h"
#include "Data/HelperBotStatsRow.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Data/HelperBotSaveData.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "BaseHelperBot.generated.h"

class UInteractableComponent;
class UHelperBotInfoWidget;
class UBaseInventoryComponent;
class UNavigationInvokerComponent;
class UNiagaraComponent;
class UParticleSystemComponent;
class UHelperBotSoundData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryInteract, AController*, InteractingController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectionStateChanged, AController*, DetectingController, bool, bIsDetected);

UCLASS()
class CR4S_API ABaseHelperBot : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseHelperBot();

#pragma region Lifecycle
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
#pragma endregion

#pragma region Interaction Functions
	
protected:
	UFUNCTION()
	void HandleInteract(AActor* InteractableActor);

protected:
	UPROPERTY()
	UMaterialInstanceDynamic* HighlightMaterialInstance = nullptr;
	
#pragma endregion

#pragma region Stats Functions
	
protected:
	void LoadStats();
	
#pragma endregion

#pragma region Interaction Variables

public:
	UPROPERTY(BlueprintAssignable)
	FOnTryInteract OnTryInteract;

	UPROPERTY(BlueprintAssignable)
	FOnDetectionStateChanged OnDetectionStateChanged;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> InteractableComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHelperBotStateManagerWidget> StateUIClass;

	UPROPERTY()
	TObjectPtr<UHelperBotStateManagerWidget> StateUIInstance = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryContainerWidget> InventoryContainerWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventoryContainerWidget> InventoryContainerWidgetInstance;
	
#pragma endregion

#pragma region Stats Variables
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName = NAME_None;

	UPROPERTY()
	FHelperBotStatsRow CurrentStats = {};

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText BotName;

private:
	FHelperPickUpData PickUpData;
	
	uint8 bIsFromInventory : 1 = 0;
	
#pragma endregion

#pragma region Getters/Setters
	
public:
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() const { return CurrentStats.MaxHealth; }
	
	FORCEINLINE float GetWoodDamagePerSecond() const { return CurrentStats.WoodDamagePerSecond; }
	FORCEINLINE float GetRockPerSecond() const { return CurrentStats.RockDamagePerSecond; }
	FORCEINLINE float GetRepairingPerSecond() const { return CurrentStats.RepairingPerSecond; }
	FORCEINLINE float GetAttackPerSecond() const { return CurrentStats.AttackPerSecond; }
	FORCEINLINE float GetHarvestDuration() const { return CurrentStats.HarvestDuration; }
	FORCEINLINE bool GetIsWorking() const { return bIsWorking; }
	FORCEINLINE FText GetBotName() const { return BotName; }
	void SetIsWorking(bool NewIsWorking) {bIsWorking = NewIsWorking;}
	
	FHelperPickUpData GetHelperBotData() const { return PickUpData; }
	
	UFUNCTION(BlueprintCallable, Category = "Bot Info")
	void SetBotName(const FText& NewName);
	
	FORCEINLINE void SetIsFromInventory(bool bFromInventory) { bIsFromInventory = bFromInventory; }

	FORCEINLINE bool InitPickData() const { return PickUpData.bIsInit; }
	void SetPickUpData(const FHelperPickUpData& InPickUpData);
	
#pragma endregion

#pragma region Component
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USplineComponent> LeftEyeWorkSpline = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<USplineComponent> RightEyeWorkSpline = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	UNiagaraComponent* LeftEyeWorkVFXComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	UNiagaraComponent* RightEyeWorkVFXComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UPointLightComponent* LeftEyeLight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UPointLightComponent* RightEyeLight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UNavigationInvokerComponent* NavInvokerComponent = nullptr;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UParticleSystemComponent* WorkTargetParticle = nullptr;

#pragma endregion

#pragma region State Visual
public:
	UFUNCTION()
	void UpdateStateVisualEffects();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	FLinearColor IdleEyeColor = FLinearColor::Blue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	FLinearColor ResourceEyeColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	FLinearColor DefendingEyeColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	FLinearColor RepairingEyeColor = FLinearColor::Yellow;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	class UNiagaraSystem* ResourceWorkVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	class UNiagaraSystem* DefendingWorkVFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "State Visual Settings")
	class UNiagaraSystem* RepairingWorkVFX = nullptr;

#pragma endregion
	
#pragma region Info Widget
	
public:
	UFUNCTION()
	void OnDetectedChange(AActor* InteractableActor, bool bIsDetected);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component")
	class UWidgetComponent* InfoWidgetComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TSubclassOf<UHelperBotInfoWidget> InfoWidgetClass;

	UPROPERTY()
	TObjectPtr<UHelperBotInfoWidget> InfoUIInstance = nullptr;
	
#pragma endregion

#pragma region Work
public:
	UFUNCTION()
	void UpdateEyeBeamWorkTarget(AActor* TargetActor);

	UFUNCTION()
	void StopEyeBeamWork();

private:
	bool bIsWorking = false;
#pragma endregion

#pragma region RobotRepair
public:
	UFUNCTION()
	bool CanRepair(APlayerCharacter* Player) const;
	
	UFUNCTION()
	bool RepairBot(APlayerCharacter* Player);
#pragma endregion
	
#pragma region Damage & Death
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage & Death")
	UNiagaraComponent* HitEffectComponent = nullptr;
		
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage & Death")
	UAnimMontage* DeathMontage = nullptr;

private:
	void PlayHitEffect(const FVector& HitDirection);
	void StartDeathSequence();

public:
	UPROPERTY(BlueprintReadOnly, Category = "Hit Effect")
	FVector SpineShakeOffset = FVector::ZeroVector;

#pragma endregion

#pragma region FadeEffect
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	FLinearColor PickUpColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects")
	FLinearColor DeadColor;
	
public:
	UFUNCTION(BlueprintCallable)
	void StartFadeOut();

private:
	float ElapsedFadeTime = 0.f;
	FTimerHandle FadeTimerHandle;
	void UpdateFadeOut();
#pragma endregion

public:
	UFUNCTION(BlueprintCallable)
	FHelperBotSaveGame GetHelperBotSaveData() const;

	UFUNCTION(BlueprintCallable)
	void LoadHelperBotSaveData(const FHelperBotSaveGame& Data);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	UHelperBotSoundData* SoundData;
	
	void PlayBotSound(USoundBase* Sound, const FVector& Location = FVector::ZeroVector) const;
};