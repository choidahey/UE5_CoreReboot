// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "AutomaticFireWeapon.generated.h"

UCLASS()
class CR4S_API AAutomaticFireWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:
	AAutomaticFireWeapon();

#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void StopAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region AutomaticFire
	void FireBulletForInterval();
#pragma endregion

#pragma region Timer
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Timer")
	FTimerHandle FireRateTimerHandle;
#pragma endregion
};
