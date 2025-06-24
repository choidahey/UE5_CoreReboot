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

bool UModularRobotStatusComponent::CheckWeightCapacity(const float AdditionalWeight) const
{
	const float TotalWeight=AdditionalWeight+RobotStatus.Weight;
	if (TotalWeight>RobotStatus.MaxWeight) return false;

	return false;
}

void UModularRobotStatusComponent::Refresh()
{
	Super::Refresh();
	AddEnergy(0);
	AddStun(0);
	AddWeight(0);
	AddMaxWeight(0);
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

	AddStun(-RobotStatus.MaxStun);
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

	if (RobotStatus.Energy<=KINDA_SMALL_NUMBER&&bIsRobotActive)
	{
		bIsRobotActive=false;
		StopConsumeEnergy();
	}
	else if (RobotStatus.Energy>=KINDA_SMALL_NUMBER&&!bIsRobotActive)
	{
		bIsRobotActive=true;
	}
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


void UModularRobotStatusComponent::SetEnergyConsumptionAmount(const float NewAmount)
{
	RobotStatus.EnergyConsumptionAmount=NewAmount;
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
	if (Percentage>=1&&!bIsStunned)
	{
		if (!CR4S_ENSURE(LogHong1,GetWorld())) return;

		bIsStunned=true;
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
	OnMaxWeightChanged.Broadcast(RobotStatus.MaxWeight);
}

void UModularRobotStatusComponent::AddWeight(const float InAmount)
{
	RobotStatus.Weight+=InAmount;
	OnWeightChanged.Broadcast(RobotStatus.Weight);
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


