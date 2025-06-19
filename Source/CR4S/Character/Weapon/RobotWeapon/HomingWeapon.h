// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "HomingWeapon.generated.h"

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
	TWeakObjectPtr<AActor> PotentialTarget;

	UPROPERTY(VisibleAnywhere, Category="LockOn")
	uint8 bIsLockingOn :1 {false};

	UPROPERTY(VisibleAnywhere, Category="LockOn")
	float LockOnStartTime {0};
#pragma endregion 
};
