// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBufferComponent.h"
#include "RobotInputBufferComponent.generated.h"


class ABaseWeapon;
class AModularRobot;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API URobotInputBufferComponent : public UInputBufferComponent
{
	GENERATED_BODY()

public:
	URobotInputBufferComponent();

	
public:
	void RefreshWeaponListByIndex(const int32 Index, ABaseWeapon* InWeapon);
	
	virtual void ExecuteInputQueue() const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TObjectPtr<ABaseWeapon>> Weapons;
};
