// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseStatusComponent.h"

#include "CR4S.h"


// Sets default values for this component's properties
UBaseStatusComponent::UBaseStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBaseStatusComponent::Refresh()
{
	AddCurrentHP(0);
	AddCurrentResource(0);
	AddHeatThreshold(0);
	AddColdThreshold(0);
	AddHumidityThreshold(0);
}

// Called when the game starts
void UBaseStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UBaseStatusComponent::SetRollStaminaCost(const float NewCost)
{
	OriginalRollStaminaCost = BaseStatus.RollStaminaCost;
	BaseStatus.RollStaminaCost=NewCost;
}

void UBaseStatusComponent::SetResourceRegenDelay(const float NewDelay)
{
	OriginalResourceRegenDelay = BaseStatus.ResourceRegenDelay;
	BaseStatus.ResourceRegenDelay=NewDelay;
}

void UBaseStatusComponent::AddMaxHP(const float InAmount)
{
	BaseStatus.MaxHealth+=InAmount;
	const float Percentage=FMath::Clamp((BaseStatus.Health)/BaseStatus.MaxHealth,0.f,1.f);
	OnHPChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::AddMaxResource(const float InAmount)
{
	BaseStatus.MaxResource+=InAmount;
	const float Percentage=FMath::Clamp((BaseStatus.Resource)/BaseStatus.MaxResource,0.f,1.f);
	OnResourceChanged.Broadcast(Percentage);
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

void UBaseStatusComponent::AddCurrentResource(const float InAmount)
{
	const float Temp=FMath::Clamp(BaseStatus.Resource+InAmount,0,BaseStatus.MaxResource);
	BaseStatus.Resource=Temp;
	const float Percentage=FMath::Clamp((BaseStatus.Resource)/BaseStatus.MaxResource,0.f,1.f);
	OnResourceChanged.Broadcast(Percentage);
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
	OnColdThresholdChanged.Broadcast(BaseStatus.ColdThreshold);
}

void UBaseStatusComponent::AddHeatThreshold(const float InAmount)
{
	BaseStatus.HeatThreshold+=InAmount;
	OnHeatThresholdChanged.Broadcast(BaseStatus.HeatThreshold);
}

void UBaseStatusComponent::AddHumidityThreshold(const float InAmount)
{
	BaseStatus.HumidityThreshold+=InAmount;
	OnHumidityThresholdChanged.Broadcast(BaseStatus.HumidityThreshold);
}

void UBaseStatusComponent::ApplyResourceRegenModifier(const float Modifier)
{
	BaseStatus.ResourceRegenMultiplier*=Modifier;
}

void UBaseStatusComponent::RevertResourceRegenModifier(const float Modifier)
{
	BaseStatus.ResourceRegenMultiplier/=Modifier;
}

void UBaseStatusComponent::ApplyResourceConsumptionModifier(const float Modifier)
{
	BaseStatus.ResourceConsumptionMultiplier*=Modifier;
}

void UBaseStatusComponent::RevertResourceConsumptionModifier(const float Modifier)
{
	BaseStatus.ResourceConsumptionMultiplier/=Modifier;
}

bool UBaseStatusComponent::HasEnoughResourceForRoll() const
{
	return GetCurrentResource()>=(BaseStatus.RollStaminaCost*BaseStatus.ResourceConsumptionMultiplier);
}

//Dash & Roll
void UBaseStatusComponent::ConsumeResourceForRoll()
{
	AddCurrentResource(-(BaseStatus.RollStaminaCost*BaseStatus.ResourceConsumptionMultiplier));
	OnResourceConsumed();
}

void UBaseStatusComponent::StartResourceRegen()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld())) return;

	GetWorld()->GetTimerManager().SetTimer(
		StaminaRegenTimerHandle,
		this,
		&UBaseStatusComponent::RegenResourceForInterval,
		BaseStatus.ResourceRegenInterval,
		true,
		BaseStatus.ResourceRegenDelay
	);
}

void UBaseStatusComponent::StopResourceRegen()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld())) return;
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
}

void UBaseStatusComponent::OnResourceConsumed()
{
	StopResourceRegen();
	StartResourceRegen();
}

void UBaseStatusComponent::RegenResourceForInterval()
{
	if (GetCurrentResource()>=GetMaxResource())
	{
		StopResourceRegen();
		return;
	}
	AddCurrentResource((BaseStatus.ResourceRegenPerInterval*BaseStatus.ResourceRegenMultiplier));
}

void UBaseStatusComponent::HandleTemperatureChanged(float NewTemperature)
{
	if (NewTemperature>BaseStatus.HeatThreshold)
	{
		if (!bIsOverHeated)
		{
			bIsOverHeated=true;
			ApplyHeatDebuff();
		}
	}
	else if (NewTemperature<BaseStatus.ColdThreshold)
	{
		if (!bIsFreezing)
		{
			bIsFreezing=true;
			ApplyColdDebuff();
		}
	}
	else
	{
		if (bIsOverHeated)
		{
			bIsOverHeated=false;
			RemoveHeatDebuff();
		}
		if (bIsFreezing)
		{
			bIsFreezing=false;
			RemoveColdDebuff();
		}
	}
}

void UBaseStatusComponent::ApplyHeatDebuff()
{
	if (GetWorld()&&BaseStatus.HeatDamageInterval>KINDA_SMALL_NUMBER)
	{
		GetWorld()->GetTimerManager().SetTimer(
			OverHeatDamageTimerHandle,
			this,
			&UBaseStatusComponent::ApplyOverHeatDamage,
			BaseStatus.HeatDamageInterval,
			true
		);
	}
}

void UBaseStatusComponent::RemoveHeatDebuff()
{
	if (CR4S_ENSURE(LogHong1,GetWorld())) return;
	GetWorld()->GetTimerManager().ClearTimer(OverHeatDamageTimerHandle);
}

void UBaseStatusComponent::ApplyOverHeatDamage()
{
	AddCurrentHP(-(BaseStatus.HeatDamageAmount));
}

void UBaseStatusComponent::ApplyColdDebuff()
{
}

void UBaseStatusComponent::RemoveColdDebuff()
{
}


void UBaseStatusComponent::HandleHumidityChanged(float NewHumidity)
{
	if (NewHumidity>BaseStatus.HumidityThreshold)
	{
		if (!bIsHumidityAffected)
		{
			bIsHumidityAffected=true;
			ApplyHighHumidityDebuff();
		}
	}
	else
	{
		if (bIsHumidityAffected)
		{
			bIsHumidityAffected=false;
			RemoveHighHumidityDebuff();
		}
	}
}

void UBaseStatusComponent::ApplyHighHumidityDebuff()
{
}

void UBaseStatusComponent::RemoveHighHumidityDebuff()
{
}

// Called every frame
void UBaseStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



