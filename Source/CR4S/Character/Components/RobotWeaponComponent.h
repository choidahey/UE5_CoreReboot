// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBufferComponent.h"
#include "Character/Data/RobotSettings.h"
#include "RobotWeaponComponent.generated.h"


class URobotInputBufferComponent;
class UInputBufferComponent;
class ABaseWeapon;
struct FGameplayTag;
class UBaseWeapon;
class AModularRobot;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API URobotWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URobotWeaponComponent();

#pragma region Check
	ABaseWeapon* GetWeaponByIndex(const int32 SlotIdx);
	FORCEINLINE bool IsDuringAttackAction() const { return bIsDuringAttackAction; }
	FORCEINLINE void SetIsDuringAttackAction(const bool bIsAttacking) { bIsDuringAttackAction= bIsAttacking; }
#pragma endregion
	
#pragma region Attack
public:
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackLeftArm();
	UFUNCTION(BlueprintCallable)
	void Input_StopAttackLeftArm();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackRightArm();
	UFUNCTION(BlueprintCallable)
	void Input_StopAttackRightArm();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackLeftShoulder();
	UFUNCTION(BlueprintCallable)
	void Input_StopAttackLeftShoulder();
	UFUNCTION(BlueprintCallable)
	void Input_OnAttackRightShoulder();
	UFUNCTION(BlueprintCallable)
	void Input_StopAttackRightShoulder();

	void TryAttackBySlot(const int32 SlotIdx, const EInputType AttackInput);
	void StopAttackBySlot(const int32 SlotIdx);
#pragma endregion
	
#pragma region EquipWeapon
public:
	void RefreshWeaponUI();
	UFUNCTION(BlueprintCallable)
	void EquipWeaponByTag(const FGameplayTag& Tag, const int32 SlotIdx);
	void UnequipWeapon(const int32 SlotIdx);
	void BindWidgetWeapon();
	bool IsArmSlot(const int32 SlotIdx) const;
#pragma endregion

#pragma region Overrides
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AModularRobot> OwningCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URobotInputBufferComponent> InputBuffer;
	
	//Left, Right Arm (0,1), Left, Right Shoulder(2,3)	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Weapons")
	TArray<TObjectPtr<ABaseWeapon>> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDuringAttackAction:1 {false};

#pragma endregion

#pragma region WeaponSettings
	FWeaponSettings WeaponSettings;
#pragma endregion
};
