// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularRobotStatusComponent.h"

#include "Character/Characters/ModularRobot.h"


// Sets default values for this component's properties
UModularRobotStatusComponent::UModularRobotStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UModularRobotStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter=Cast<AModularRobot>(GetOwner());

	if (StatusData)
	{
		BaseStatus=StatusData->BaseStats;
		RobotStatus=StatusData->RobotStats;
	}
}

// Called every frame
void UModularRobotStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
												 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UModularRobotStatusComponent::AddMaxEnergy(const float InAmount)
{
	RobotStatus.MaxEnergy+=InAmount;
	const float Percentage=FMath::Clamp(RobotStatus.Energy/RobotStatus.MaxEnergy,0.f,1.f);
	OnEnergyChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::AddEnergy(const float InAmount)
{
	const float Temp=FMath::Clamp(RobotStatus.Energy+InAmount,0.f,RobotStatus.MaxEnergy);
	RobotStatus.Energy=Temp;
	const float Percentage=FMath::Clamp(RobotStatus.Energy/RobotStatus.MaxEnergy,0.f,1.f);
	OnEnergyChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::AddMaxStun(const float InAmount)
{
	RobotStatus.MaxStun+=InAmount;
	const float Percentage=FMath::Clamp(RobotStatus.Stun/RobotStatus.MaxStun,0.f,1.f);
	OnStunChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::AddStun(const float InAmount)
{
	const float Temp=FMath::Clamp(RobotStatus.Stun+InAmount,0.f,RobotStatus.MaxStun);
	RobotStatus.Stun=Temp;
	const float Percentage=FMath::Clamp(RobotStatus.Stun/RobotStatus.MaxStun,0.f,1.f);
	OnStunChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::AddStunResistance(const float InAmount)
{
	RobotStatus.StunResistance+=InAmount;
}

void UModularRobotStatusComponent::AddArmorMultiplier(const float InAmount)
{
	RobotStatus.ArmorMultiplier+=InAmount;
}

void UModularRobotStatusComponent::AddAttackPowerMultiplier(const float InAmount)
{
	RobotStatus.AttackPowerMultiplier+=InAmount;
}

void UModularRobotStatusComponent::AddMaxWeight(const float InAmount)
{
	RobotStatus.MaxWeight+=InAmount;
	const float Percentage=FMath::Clamp(RobotStatus.Weight/RobotStatus.MaxWeight,0.f,1.f);
	OnWeightChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::AddWeight(const float InAmount)
{
	const float Temp=FMath::Clamp(RobotStatus.Weight+InAmount,0.f,RobotStatus.MaxWeight);
	RobotStatus.Weight=Temp;
	const float Percentage=FMath::Clamp(RobotStatus.Weight/RobotStatus.MaxWeight,0.f,1.f);
	OnWeightChanged.Broadcast(Percentage);
}


