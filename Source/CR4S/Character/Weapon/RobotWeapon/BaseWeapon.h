// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Data/WeaponData.h"
#include "Character/Weapon/BaseTool.h"
#include "UObject/Object.h"
#include "BaseWeapon.generated.h"

class URobotInputBufferComponent;
struct FBaseWeaponInfo;
class AModularRobot;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class CR4S_API ABaseWeapon : public ABaseTool
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	FORCEINLINE bool IsSelfStunWeapon() const { return BaseInfo.bHasSelfStun; }
	
	FORCEINLINE float GetWeaponWeight() const { return BaseInfo.Weight; }
	
	virtual void Initialize(AModularRobot* OwnerCharacter, const int32 SlotIdx);
#pragma region Attack
public:
	virtual int32 GetCurrentAmmo() const;
	virtual void SetCurrentAmmo(const int32 NewAmmo);
	
	virtual float ComputeFinalDamage() override;
	virtual void OnAttack() override;
	virtual void StopAttack();
protected:
	void StartAttackCooldown();
	void ResetAttackCooldown();
	void ApplySelfStun() const;
	void RemoveSelfStun() const;
#pragma endregion

	
#pragma region WeaponInfo
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Tags")
	FBaseWeaponInfo BaseInfo;
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	TObjectPtr<AModularRobot> OwningCharacter;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	TObjectPtr<URobotInputBufferComponent> InputBuffer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Owner")
	uint8 bCanAttack:1 {true};

	FTimerHandle AttackCooldownTimerHandler;	
#pragma endregion

#pragma region Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> SceneComp;
#pragma endregion
};
