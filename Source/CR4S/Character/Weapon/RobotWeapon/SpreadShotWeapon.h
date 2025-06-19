// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangedWeapon.h"
#include "SpreadShotWeapon.generated.h"

UCLASS()
class CR4S_API ASpreadShotWeapon : public ARangedWeapon
{
	GENERATED_BODY()

public:
	ASpreadShotWeapon();

#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion
};
