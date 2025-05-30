// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatusComponent.h"


// Sets default values for this component's properties
UBaseStatusComponent::UBaseStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UBaseStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UBaseStatusComponent::AddMaxHP(const float InAmount)
{
	BaseStatus.MaxHealth+=InAmount;
	const float Percentage=FMath::Clamp((BaseStatus.Health)/BaseStatus.MaxHealth,0.f,1.f);
	OnHPChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::AddMaxEnergy(const float InAmount)
{
	BaseStatus.MaxEnergy+=InAmount;
	const float Percentage=FMath::Clamp((BaseStatus.Energy)/BaseStatus.MaxEnergy,0.f,1.f);
	OnEnergyChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::AddCurrentHP(const float InAmount)
{
	const float Temp=FMath::Clamp(BaseStatus.Health+InAmount,0,BaseStatus.MaxHealth);
	if (Temp <= 0)
	{
		BaseStatus.Health = 0;
		OnDeathState.Broadcast();
	}
	else
	{
		BaseStatus.Health=Temp;
	}
	const float Percentage=FMath::Clamp((BaseStatus.Health)/BaseStatus.MaxHealth,0.f,1.f);
	OnHPChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::AddCurrentEnergy(const float InAmount)
{
	const float Temp=FMath::Clamp(BaseStatus.Energy+InAmount,0,BaseStatus.Energy);
	BaseStatus.Energy=Temp;
	const float Percentage=FMath::Clamp((BaseStatus.Energy)/BaseStatus.MaxEnergy,0.f,1.f);
	OnEnergyChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::AddEnergyConsumptionRate(const float InAmount)
{
	BaseStatus.EnergyConsumptionRate+=InAmount;
}

void UBaseStatusComponent::AddAttackPower(const float InAmount)
{
	BaseStatus.AttackPower+=InAmount;
}

void UBaseStatusComponent::AddArmor(const float InAmount)
{
	BaseStatus.Armor+=InAmount;
}

void UBaseStatusComponent::AddColdThreshold(const float InAmount)
{
	BaseStatus.ColdThreshold+=InAmount;
}

void UBaseStatusComponent::AddHeatThreshold(const float InAmount)
{
	BaseStatus.HeatThreshold+=InAmount;
}

void UBaseStatusComponent::AddHumidityThreshold(const float InAmount)
{
	BaseStatus.HumidityThreshold+=InAmount;
}

// Called every frame
void UBaseStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



