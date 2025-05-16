// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacterStatusComponent.h"

#include "Character/Characters/PlayerCharacter.h"


// Sets default values for this component's properties
UPlayerCharacterStatusComponent::UPlayerCharacterStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPlayerCharacterStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());

	if (StatusData)
	{
		Status=StatusData->StatsData;
	}
}


void UPlayerCharacterStatusComponent::AddMaxHP(const int32 InAmount)
{
	Status.MaxHealth+=InAmount;
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Health)/Status.MaxHealth,0.f,1.f);
	OnHPChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddMaxHunger(const int32 InAmount)
{
	Status.MaxHunger+=InAmount;
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Hunger)/Status.MaxHunger,0.f,1.f);
	OnHungerChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddMaxStamina(const int32 InAmount)
{
	Status.MaxStamina+=InAmount;
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Stamina)/Status.MaxStamina,0.f,1.f);
	OnStaminaChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddCurrentHP(const int32 InAmount)
{
	const int32 Temp=FMath::Clamp(Status.Health+InAmount,0,Status.MaxHealth);
	if (Temp <= 0)
	{
		Status.Health = 0;
		OnDeathState.Broadcast();
	}
	else
	{
		Status.Health=Temp;
	}
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Health)/Status.MaxHealth,0.f,1.f);
	OnHPChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddCurrentHunger(const int32 InAmount)
{
	const int32 Temp=FMath::Clamp(Status.Hunger+InAmount,0,Status.MaxHunger);
	Status.Hunger=Temp;
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Hunger)/Status.MaxHunger,0.f,1.f);
	OnHungerChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddCurrentStamina(const int32 InAmount)
{
	const int32 Temp=FMath::Clamp(Status.Stamina+InAmount,0,Status.Stamina);
	Status.Stamina=Temp;
	const float Percentage=FMath::Clamp(static_cast<float>(Status.Stamina)/Status.MaxStamina,0.f,1.f);
	OnStaminaChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddAttackPower(const int32 InAmount)
{
	Status.AttackPower+=InAmount;
}

void UPlayerCharacterStatusComponent::AddArmor(const int32 InAmount)
{
	Status.Armor+=InAmount;
}

void UPlayerCharacterStatusComponent::AddColdThreshold(const int32 InAmount)
{
	Status.ColdThreshold+=InAmount;
}

void UPlayerCharacterStatusComponent::AddHeatThreshold(const int32 InAmount)
{
	Status.HeatThreshold+=InAmount;
}

void UPlayerCharacterStatusComponent::AddHumidityThreshold(const int32 InAmount)
{
	Status.HumidityThreshold+=InAmount;
}

// Called every frame
void UPlayerCharacterStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

