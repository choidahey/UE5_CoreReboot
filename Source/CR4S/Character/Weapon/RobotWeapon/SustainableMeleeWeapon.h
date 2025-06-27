// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeWeapon.h"
#include "SustainableMeleeWeapon.generated.h"

UCLASS()
class CR4S_API ASustainableMeleeWeapon : public AMeleeWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASustainableMeleeWeapon();

#pragma region Attack
	virtual void OnAttack() override;;
	virtual void StopAttack() override;
#pragma endregion
	
#pragma region Override
public:
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion
	
};
