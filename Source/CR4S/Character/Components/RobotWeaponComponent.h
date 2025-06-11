// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Data/RobotSettings.h"
#include "RobotWeaponComponent.generated.h"


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
	void EquipWeaponByTag(const FGameplayTag& Tag, const int32 SlotIdx);
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
	TObjectPtr<UInputBufferComponent> InputBuffer;
	
	//Left, Right Arm (0,1), Left, Right Shoulder(2,3)	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Weapons")
	TArray<TObjectPtr<ABaseWeapon>> Weapons; 
	
	int32 ActivatedWeaponIdx {-1};
#pragma endregion

#pragma region WeaponSettings
	FWeaponSettings WeaponSettings;
#pragma endregion
};
