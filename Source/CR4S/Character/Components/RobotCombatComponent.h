// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "RobotCombatComponent.generated.h"


struct FGameplayTag;
class UBaseWeapon;
class AModularRobot;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API URobotCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	URobotCombatComponent();
	
#pragma region Attack & Weapons
public:
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackLeftArm();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackRightArm();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackLeftShoulder();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackRightShoulder();

	UFUNCTION(BlueprintCallable)
	void EquipWeaponByTag(const FGameplayTag Tag, const int32 SlotIdx);
#pragma endregion

#pragma region Overrides
public:
	virtual void PerformWeaponTrace() override;
	virtual void SetWeaponTrace(const bool Trace) override;
	virtual void ExecuteInputQueue() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region Weapon
protected:
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Weapons")
	TArray<TObjectPtr<UBaseWeapon>> Weapons; //Left, Right Arm (0,1), Left, Right Shoulder(2,3)
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AModularRobot> OwningCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ActivatedWeaponIdx {-1};
#pragma endregion
	
};
