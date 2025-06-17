#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HelperBotState.h"
#include "Data/HelperBotStatsRow.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Components/SplineComponent.h"
#include "NiagaraComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "BaseHelperBot.generated.h"

class UInteractableComponent;
class UHelperBotInfoWidget;
class UBaseInventoryComponent;
class UNavigationInvokerComponent;
class UNiagaraComponent;
class UParticleSystemComponent;

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
	
#pragma endregion

#pragma region Getters/Setters
	
public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetWoodDamagePerSecond() const { return CurrentStats.WoodDamagePerSecond; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetRockPerSecond() const { return CurrentStats.RockDamagePerSecond; }
	
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetRepairingPerSecond() const { return CurrentStats.RepairingPerSecond; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	FORCEINLINE bool GetIsWorking() const { return bIsWorking; }

	void SetIsWorking(bool NewIsWorking) {bIsWorking = NewIsWorking;}
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
	
#pragma region Info Widget
	
public:
	UFUNCTION()
	void OnDetectedChange(AActor* InteractableActor, bool bIsDetected);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHelperBotInfoWidget> InfoUIClass;

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
	

};