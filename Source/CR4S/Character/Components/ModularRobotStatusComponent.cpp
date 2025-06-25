// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularRobotStatusComponent.h"

#include "CR4S.h"
#include "Character/Characters/ModularRobot.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values for this component's properties
UModularRobotStatusComponent::UModularRobotStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UModularRobotStatusComponent::CheckTotalWeightCapacity()
{
	bExceedsTotalWeightLimit = (RobotStatus.Weight>RobotStatus.MaxWeight) ? true : false;
}

void UModularRobotStatusComponent::CheckArmCapacity()
{
	bExceedsArmWeightLimit = (RobotStatus.CurrentArmMountWeight>RobotStatus.MaxArmMountWeight) ? true : false;
}

void UModularRobotStatusComponent::Refresh()
{
	Super::Refresh();
	AddEnergy(0);
	AddStun(0);
	AddCurrentWeight(0);
	AddMaxWeight(0);
	AddCurrentArmMountWeight(0);
	AddMaxArmMountWeight(0);
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

void UModularRobotStatusComponent::RemoveStunDebuff()
{
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	SetCurrentStun(0);
	OwningCharacter->SetInputEnable(true);
	bIsStunned=false;
}

void UModularRobotStatusComponent::StartConsumeEnergy()
{
	if (RobotStatus.EnergyConsumptionInterval>KINDA_SMALL_NUMBER&&GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			EnergyTimerHandle,
			this,
			&UModularRobotStatusComponent::ConsumeEnergyForInterval,
			RobotStatus.EnergyConsumptionInterval,
			true
		);
	}
}

void UModularRobotStatusComponent::StopConsumeEnergy()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld())) return;

	GetWorld()->GetTimerManager().ClearTimer(EnergyTimerHandle);
}

void UModularRobotStatusComponent::ConsumeEnergyForInterval()
{
	AddEnergy(-(RobotStatus.EnergyConsumptionAmount*RobotStatus.EnergyEfficiency));
}

void UModularRobotStatusComponent::ApplyHeatDebuff()
{
	Super::ApplyHeatDebuff();
}

void UModularRobotStatusComponent::RemoveHeatDebuff()
{
	Super::RemoveHeatDebuff();
}

void UModularRobotStatusComponent::ApplyColdDebuff()
{
	BaseStatus.ResourceConsumptionMultiplier*=RobotStatus.ColdResourceConsumptionMultiplier;
}

void UModularRobotStatusComponent::RemoveColdDebuff()
{
	BaseStatus.ResourceConsumptionMultiplier/=RobotStatus.ColdResourceConsumptionMultiplier;
}

void UModularRobotStatusComponent::ApplyHighHumidityDebuff()
{
	RobotStatus.AttackPowerMultiplier*=RobotStatus.HighHumidityAttackPowerMultiplier;
}

void UModularRobotStatusComponent::RemoveHighHumidityDebuff()
{
	RobotStatus.AttackPowerMultiplier/=RobotStatus.HighHumidityAttackPowerMultiplier;
}

// Called every frame
void UModularRobotStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
												 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


float UModularRobotStatusComponent::GetCurrentEnergyPercentage() const
{
	if (RobotStatus.MaxEnergy<=KINDA_SMALL_NUMBER)
	{
		return 0;
	}
	return FMath::Clamp(RobotStatus.Energy/RobotStatus.MaxEnergy,0,1);
}

float UModularRobotStatusComponent::GetCurrentStunPercentage() const
{
	if (RobotStatus.MaxStun<=KINDA_SMALL_NUMBER)
	{
		return 0;
	}
	return FMath::Clamp(RobotStatus.Stun/RobotStatus.MaxStun,0,1);
}

void UModularRobotStatusComponent::SetMaxEnergy(const float NewValue)
{
	RobotStatus.MaxEnergy=NewValue;
	const float Percentage = GetCurrentEnergyPercentage();
	OnEnergyChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::SetCurrentEnergy(const float NewValue)
{
	const float ClampedEnergy = FMath::Clamp(NewValue, 0.f, RobotStatus.MaxEnergy);
	const bool bValueChanged= !FMath::IsNearlyEqual(RobotStatus.Energy, ClampedEnergy);
	
	RobotStatus.Energy = ClampedEnergy;
	
	const float Percentage = GetCurrentEnergyPercentage();
	OnEnergyChanged.Broadcast(Percentage);

	if (!bValueChanged) return;
	
	if (RobotStatus.Energy <= KINDA_SMALL_NUMBER)
	{
		if (bIsRobotActive)
		{
			bIsRobotActive = false;
			StopConsumeEnergy();
		}
	}
	else
	{
		if (!bIsRobotActive)
		{
			bIsRobotActive = true;
		}
	}
}

void UModularRobotStatusComponent::SetMaxStun(const float NewValue)
{
	RobotStatus.MaxStun=NewValue;
	const float Percentage = GetCurrentStunPercentage();
	OnStunChanged.Broadcast(Percentage);
}

void UModularRobotStatusComponent::SetCurrentStun(const float NewValue)
{
	const float ClampedStun = FMath::Clamp(NewValue, 0.f, RobotStatus.MaxStun);
	const bool bValueChanged= !FMath::IsNearlyEqual(RobotStatus.Stun, ClampedStun);

	RobotStatus.Stun = ClampedStun;
	const float Percentage = GetCurrentStunPercentage();
	OnStunChanged.Broadcast(Percentage);

	if (!bValueChanged) return;

	if (Percentage >= 1.f && !bIsStunned)
	{
		if (!CR4S_ENSURE(LogHong1, GetWorld() && OwningCharacter)) return;
		
		bIsStunned = true;
		OwningCharacter->SetInputEnable(false);
		
		GetWorld()->GetTimerManager().ClearTimer(StunTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			StunTimerHandle,
			this,
			&UModularRobotStatusComponent::RemoveStunDebuff,
			RobotStatus.StunDuration,
			false
		);
	}
}

void UModularRobotStatusComponent::SetArmorMultiplier(const float NewValue)
{
	RobotStatus.ArmorMultiplier=NewValue;
}

void UModularRobotStatusComponent::SetAttackPowerMultiplier(const float NewValue)
{
	RobotStatus.AttackPowerMultiplier=NewValue;
}

void UModularRobotStatusComponent::SetMaxWeight(const float NewValue)
{
	RobotStatus.MaxWeight=NewValue;
	OnMaxWeightChanged.Broadcast(RobotStatus.MaxWeight);
	CheckTotalWeightCapacity();
}

void UModularRobotStatusComponent::SetCurrentWeight(const float NewValue)
{
	RobotStatus.Weight=NewValue;
	OnWeightChanged.Broadcast(RobotStatus.Weight);
	CheckTotalWeightCapacity();
}

void UModularRobotStatusComponent::SetMaxArmMountWeight(const float NewValue)
{
	RobotStatus.MaxArmMountWeight=NewValue;
	OnMaxArmLoadChanged.Broadcast(RobotStatus.MaxArmMountWeight);
	CheckArmCapacity();
}

void UModularRobotStatusComponent::SetCurrentArmMountWeight(const float NewValue)
{
	RobotStatus.CurrentArmMountWeight=NewValue;
	OnArmLoadChanged.Broadcast(RobotStatus.CurrentArmMountWeight);
	CheckArmCapacity();
}

void UModularRobotStatusComponent::SetEnergyConsumptionAmount(const float NewValue)
{
	RobotStatus.EnergyConsumptionAmount=NewValue;
}

void UModularRobotStatusComponent::AddMaxEnergy(const float InAmount)
{
	SetMaxEnergy(RobotStatus.MaxEnergy+InAmount);
}

void UModularRobotStatusComponent::AddEnergy(const float InAmount)
{
	SetCurrentEnergy(RobotStatus.Energy+InAmount);
}

void UModularRobotStatusComponent::AddMaxStun(const float InAmount)
{
	SetMaxStun(RobotStatus.MaxStun+InAmount);
}

void UModularRobotStatusComponent::AddStun(const float InAmount)
{
	SetCurrentStun(RobotStatus.Stun+InAmount);
}

void UModularRobotStatusComponent::AddArmorMultiplier(const float InAmount)
{
	SetArmorMultiplier(RobotStatus.ArmorMultiplier+InAmount);
}

void UModularRobotStatusComponent::AddAttackPowerMultiplier(const float InAmount)
{
	SetAttackPowerMultiplier(RobotStatus.AttackPowerMultiplier+InAmount);
}

void UModularRobotStatusComponent::AddMaxWeight(const float InAmount)
{
	SetMaxWeight(RobotStatus.MaxWeight+InAmount);
}

void UModularRobotStatusComponent::AddCurrentWeight(const float InAmount)
{
	SetCurrentWeight(RobotStatus.Weight+InAmount);
}

void UModularRobotStatusComponent::AddMaxArmMountWeight(const float InAmount)
{
	SetMaxArmMountWeight(RobotStatus.MaxArmMountWeight+InAmount);
}

void UModularRobotStatusComponent::AddCurrentArmMountWeight(const float InAmount)
{
	SetCurrentArmMountWeight(RobotStatus.CurrentArmMountWeight+InAmount);
}

void UModularRobotStatusComponent::ApplyEnergyEfficiency(const float Modifier)
{
	RobotStatus.EnergyEfficiency*=Modifier;
}

void UModularRobotStatusComponent::RevertEnergyEfficiency(const float Modifier)
{
	RobotStatus.EnergyEfficiency/=Modifier;
}

void UModularRobotStatusComponent::ApplyRecoilModifier(const float Modifier)
{
	RobotStatus.RecoilModifier*=Modifier;
}

void UModularRobotStatusComponent::RevertRecoilModifier(const float Modifier)
{
	RobotStatus.RecoilModifier/=Modifier;
}

void UModularRobotStatusComponent::ApplyMeleeDamageModifier(const float Modifier)
{
	RobotStatus.MeleeDamageModifier*=Modifier;
}

void UModularRobotStatusComponent::RevertMeleeDamageModifier(const float Modifier)
{
	RobotStatus.MeleeDamageModifier/=Modifier;
}

void UModularRobotStatusComponent::StartHover()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld() && OwningCharacter)) return;

	if (CR4S_ENSURE(LogHong1,GetCurrentResource()>=RobotStatus.HoverCostMultiplier))
	{
		OwningCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetWorld()->GetTimerManager().SetTimer(
			HoverTimerHandle,
			this,
			&UModularRobotStatusComponent::ConsumeResourceForHovering,
			RobotStatus.HoverResourceConsumptionInterval,
			true
		);
	}
}

void UModularRobotStatusComponent::StopHover()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld()&&OwningCharacter)) return;
	
	OwningCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetWorld()->GetTimerManager().ClearTimer(HoverTimerHandle);
}

void UModularRobotStatusComponent::ConsumeResourceForHovering()
{
	const float HoverCost=RobotStatus.HoverCostMultiplier*BaseStatus.ResourceConsumptionAmount*BaseStatus.ResourceConsumptionMultiplier;
	if (GetCurrentResource()<HoverCost)
	{
		StopHover();
		return;
	}
	
	AddCurrentResource(-(HoverCost));
	OnResourceConsumed();
}


