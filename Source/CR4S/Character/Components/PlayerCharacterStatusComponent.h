// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseStatusComponent.h"
#include "Character/Data/PlayerCharacterStatus.h"
#include "PlayerCharacterStatusComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHungerChangedDelegate, float /*InPercentage*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHungerDebuffChanged, bool, bIsStarving);

class UPlayerCharacterStatusAssets;
class APlayerCharacter;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlayerCharacterStatusComponent : public UBaseStatusComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCharacterStatusComponent();
#pragma region Get
	FORCEINLINE float GetMaxHunger() const { return PlayerStatus.MaxHunger; }
	FORCEINLINE float GetCurrentHunger() const { return PlayerStatus.Hunger; }
#pragma endregion

#pragma region Hunger
	void AddMaxHunger(const float InAmount);
	void AddCurrentHunger(const float InAmount);

	void ApplyStarvationDamage();
	UFUNCTION()
	void ApplyHungerDebuff();
	UFUNCTION()
	void RemoveHungerDebuff();
	void ConsumeCurrentHunger();
	FORCEINLINE bool IsStarving() const { return bIsStarving; }
#pragma endregion
	
#pragma region Sprint
	void StartSprint();
	void StopSprint();
	void ConsumeResourceForSprint();
#pragma endregion
	
#pragma region Override
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

	
#pragma region DataAsset
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="StatusData")
	TObjectPtr<UPlayerCharacterStatusAsset> StatusData; 
#pragma endregion

#pragma region Owner
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<APlayerCharacter> OwningCharacter;
#pragma endregion
	
#pragma region Status
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FPlayerCharacterStats PlayerStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	uint8 bIsStarving:1 {false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FTimerHandle HungerTimerHandle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FTimerHandle StarvationDamageTimerHandle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FTimerHandle SprintTimerHandle;
#pragma endregion

#pragma region Delegate
public:
	FOnHungerChangedDelegate OnHungerChanged;
	UPROPERTY(EditAnywhere, BlueprintAssignable)
	FOnHungerDebuffChanged OnHungerDebuffChanged;
#pragma endregion
};




