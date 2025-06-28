// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeleeWeapon.h"
#include "ChargableMeleeWeapon.generated.h"

UCLASS()
class CR4S_API AChargableMeleeWeapon : public AMeleeWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChargableMeleeWeapon();

#pragma region Override
public:
	virtual void OnAttack() override;
	virtual void StopAttack() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Cached
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackPressTime {0};
#pragma endregion
	
};
