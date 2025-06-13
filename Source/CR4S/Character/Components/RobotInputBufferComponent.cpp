// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotInputBufferComponent.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/BaseWeapon.h"


// Sets default values for this component's properties
URobotInputBufferComponent::URobotInputBufferComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Weapons.SetNum(4);
}

void URobotInputBufferComponent::RefreshWeaponListByIndex(const int32 Index, ABaseWeapon* InWeapon)
{
	if (!CR4S_ENSURE(LogHong1,Weapons.IsValidIndex(Index))) return;

	Weapons[Index]=InWeapon;
}

void URobotInputBufferComponent::ExecuteInputQueue() const
{
	switch (CurrentInputQueue)
	{
	case EInputType::None:
		break;
	case EInputType::RobotAttack1:
		if (!CR4S_ENSURE(LogHong1,(Weapons.IsValidIndex(0)||IsValid(Weapons[0])))) return;
		Weapons[0]->OnAttack();
		break;
	case EInputType::RobotAttack2:
		if (!CR4S_ENSURE(LogHong1,(Weapons.IsValidIndex(1)||IsValid(Weapons[1])))) return;
		Weapons[1]->OnAttack();
		break;
	default:
		break;
	}
}


// Called when the game starts
void URobotInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void URobotInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

