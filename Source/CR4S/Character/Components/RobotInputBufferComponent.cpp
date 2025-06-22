// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotInputBufferComponent.h"

#include "CR4S.h"
#include "RobotWeaponComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/RobotWeapon/BaseWeapon.h"


// Sets default values for this component's properties
URobotInputBufferComponent::URobotInputBufferComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void URobotInputBufferComponent::ExecuteInputQueue() const
{
	if (!CR4S_ENSURE(LogHong1,CachedWeaponComponent)) return;
	
	switch (CurrentInputQueue)
	{
	case EInputType::RobotAttack1:
		CachedWeaponComponent->Input_OnAttackLeftArm();
		break;
	case EInputType::RobotAttack2:
		CachedWeaponComponent->Input_OnAttackRightArm();
		break;
	case EInputType::RobotAttack3:
		CachedWeaponComponent->Input_OnAttackLeftShoulder();
		break;
	case EInputType::RobotAttack4:
		CachedWeaponComponent->Input_OnAttackRightShoulder();
		break;
	default:
		break;
	}
}

void URobotInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
	AActor* OnwerActor=GetOwner();
	if (!OnwerActor) return;

	CachedWeaponComponent=OnwerActor->FindComponentByClass<URobotWeaponComponent>();
	CR4S_ENSURE(LogHong1,CachedWeaponComponent);
}

void URobotInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

