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


float UBaseStatusComponent::GetCurrentHPPercentage() const
{
	if (BaseStatus.MaxHealth<=KINDA_SMALL_NUMBER)
	{
		return 0;
	}
	return FMath::Clamp(BaseStatus.Health/BaseStatus.MaxHealth,0.0,1);
}

float UBaseStatusComponent::GetCurrentResourcePercentage() const
{
	if (BaseStatus.MaxResource<=KINDA_SMALL_NUMBER)
	{
		return 0;
	}
	return FMath::Clamp(BaseStatus.Resource/BaseStatus.MaxResource,0.0,1);
}

void UBaseStatusComponent::SetMaxHP(const float NewValue)
{
	BaseStatus.MaxHealth = NewValue;

	const float Percentage=GetCurrentHPPercentage();
	OnHPChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::SetCurrentHP(const float NewValue)
{
	const float ClampedHP=FMath::Clamp(NewValue,0,BaseStatus.MaxHealth);
	const bool bValueChanged= !FMath::IsNearlyEqual(BaseStatus.Health,ClampedHP);
	
	BaseStatus.Health=ClampedHP;
	
	const float Percentage=GetCurrentHPPercentage();
	OnHPChanged.Broadcast(Percentage);

	if (!bValueChanged) return;
	
	if (BaseStatus.Health <= 0)
	{
		OnDeathState.Broadcast();
	}
}

void UBaseStatusComponent::SetMaxResource(const float NewValue)
{
	BaseStatus.MaxResource = NewValue;
	const float Percentage = GetCurrentResourcePercentage();
	OnResourceChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::SetCurrentResource(const float NewValue)
{
	const float ClampedResource = FMath::Clamp(NewValue, 0.f, BaseStatus.MaxResource);

	BaseStatus.Resource = ClampedResource;
	
	const float Percentage = GetCurrentResourcePercentage();
	OnResourceChanged.Broadcast(Percentage);
}

void UBaseStatusComponent::SetArmor(const float NewValue)
{
	BaseStatus.Armor = NewValue;
}

void UBaseStatusComponent::SetAttackPower(const float NewValue)
{
	BaseStatus.AttackPower=NewValue;
}

void UBaseStatusComponent::SetColdThreshold(const float NewValue)
{
	BaseStatus.ColdThreshold=NewValue;
	OnColdThresholdChanged.Broadcast(BaseStatus.ColdThreshold);
}

void UBaseStatusComponent::SetHeatThreshold(const float NewValue)
{
	BaseStatus.HeatThreshold=NewValue;
	OnHeatThresholdChanged.Broadcast(BaseStatus.HeatThreshold);
}

void UBaseStatusComponent::SetHumidityThreshold(const float NewValue)
{
	BaseStatus.HumidityThreshold=NewValue;
	OnHumidityThresholdChanged.Broadcast(BaseStatus.HumidityThreshold);
}

void UBaseStatusComponent::SetResourceConsumptionAmount(const float NewCost)
{
	BaseStatus.ResourceConsumptionAmount=NewCost;
}

void UBaseStatusComponent::SetResourceRegenDelay(const float NewDelay)
{
	BaseStatus.ResourceRegenDelay=NewDelay;
}

void UBaseStatusComponent::AddMaxHP(const float InAmount)
{
	SetMaxHP(BaseStatus.MaxHealth+InAmount);
}

void UBaseStatusComponent::AddCurrentHP(const float InAmount)
{
	SetCurrentHP(BaseStatus.Health+InAmount);
}

void UBaseStatusComponent::AddMaxResource(const float InAmount)
{
	SetMaxResource(BaseStatus.MaxResource+InAmount);
}

void UBaseStatusComponent::AddCurrentResource(const float InAmount)
{
	SetCurrentResource(BaseStatus.Resource+InAmount);
}

void UBaseStatusComponent::AddAttackPower(const float InAmount)
{
	SetAttackPower(BaseStatus.AttackPower+InAmount);
}

void UBaseStatusComponent::AddArmor(const float InAmount)
{
	SetArmor(BaseStatus.Armor+InAmount);
}

void UBaseStatusComponent::AddColdThreshold(const float InAmount)
{
	SetColdThreshold(BaseStatus.ColdThreshold+InAmount);
}

void UBaseStatusComponent::AddHeatThreshold(const float InAmount)
{
	SetHeatThreshold(BaseStatus.HeatThreshold+InAmount);
}

void UBaseStatusComponent::AddHumidityThreshold(const float InAmount)
{
	SetHumidityThreshold(BaseStatus.HumidityThreshold+InAmount);
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
	return GetCurrentResource()>=(BaseStatus.ResourceConsumptionAmount*BaseStatus.ResourceConsumptionMultiplier);
}

//Dash & Roll
void UBaseStatusComponent::ConsumeResourceForRoll()
{
	AddCurrentResource(-(BaseStatus.ResourceConsumptionAmount*BaseStatus.ResourceConsumptionMultiplier));
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



