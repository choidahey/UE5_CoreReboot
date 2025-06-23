// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBufferComponent.h"
#include "RobotInputBufferComponent.generated.h"


class URobotWeaponComponent;
class ABaseWeapon;
class AModularRobot;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API URobotInputBufferComponent : public UInputBufferComponent
{
	GENERATED_BODY()

public:
	URobotInputBufferComponent();

	FORCEINLINE void SetWeaponComponent(URobotWeaponComponent* InWeaponComp) { CachedWeaponComponent=InWeaponComp; }
	
#pragma region Override
public:
	virtual void ExecuteInputQueue() const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Cached
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<URobotWeaponComponent> CachedWeaponComponent;
#pragma endregion
};
