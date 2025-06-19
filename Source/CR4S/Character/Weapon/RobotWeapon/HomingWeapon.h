// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "HomingWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnSucceeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnCanceled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryingToLockOn, FVector2D, TargetScreenLocation);

UCLASS()
class CR4S_API AHomingWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHomingWeapon();

#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void StopAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Fire
	void FireHomingBullet();
#pragma endregion

#pragma region LockOn
	UPROPERTY(VisibleAnywhere, Category="LockOn")
	TWeakObjectPtr<AActor> TrackingTarget;

	UPROPERTY(VisibleAnywhere, Category="LockOn")
	float LockOnDurationCounter {0};
	
	UPROPERTY(VisibleAnywhere, Category="LockOn")
	uint8 bIsLockedOn :1 {false};

	UPROPERTY(VisibleAnywhere, Category="LockOn")
	uint8 bIsTryingToLockOn :1 {false};
#pragma endregion

#pragma region Delegate
	UPROPERTY(VisibleAnywhere,BlueprintAssignable,Category="Delegate")
	FOnTryingToLockOn OnTryingToLockOn;
	UPROPERTY(VisibleAnywhere,BlueprintAssignable,Category="Delegate")
	FOnLockOnCanceled OnLockOnCanceled;
	UPROPERTY(VisibleAnywhere,BlueprintAssignable,Category="Delegate")
	FOnLockOnSucceeded OnLockOnSucceeded;
#pragma endregion
};
