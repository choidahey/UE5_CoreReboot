// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "BurstShotWeapon.generated.h"

UCLASS()
class CR4S_API ABurstShotWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:
	ABurstShotWeapon();

#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Fire
protected:
	void FireBurstShot();
#pragma endregion

#pragma region Cached
private:
	FTimerHandle BurstTimerHandle;

	int32 BurstShotsRemaining{0};
#pragma endregion
};
