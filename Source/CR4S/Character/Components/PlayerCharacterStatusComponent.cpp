// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacterStatusComponent.h"
#include "CR4S.h"
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
		BaseStatus=StatusData->BaseStats;
		PlayerStatus=StatusData->PlayerStats;
	}

	StartConsumeHunger();
}

// Called every frame
void UPlayerCharacterStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerCharacterStatusComponent::ApplyStarvationDamage()
{
	AddCurrentHP(-PlayerStatus.StarvationDamage);
}

void UPlayerCharacterStatusComponent::StartConsumeHunger()
{
	if (PlayerStatus.HungerInterval>KINDA_SMALL_NUMBER&&GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			HungerTimerHandle,
			this,
			&UPlayerCharacterStatusComponent::ConsumeCurrentHungerForInterval,
			PlayerStatus.HungerInterval,
			true
		);
	}
}

void UPlayerCharacterStatusComponent::ApplyHungerDebuff()
{
	if (!OwningCharacter) return;
	
	if (PlayerStatus.StarvationDamageInterval>KINDA_SMALL_NUMBER)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(StarvationDamageTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(
				StarvationDamageTimerHandle,
				this,
				&UPlayerCharacterStatusComponent::ApplyStarvationDamage,
				PlayerStatus.StarvationDamageInterval,
				true
			);
		}
	}
}

void UPlayerCharacterStatusComponent::RemoveHungerDebuff()
{
	if (CR4S_ENSURE(LogHong1,!OwningCharacter||!GetWorld())) return;

	GetWorld()->GetTimerManager().ClearTimer(StarvationDamageTimerHandle);
}

void UPlayerCharacterStatusComponent::ConsumeCurrentHungerForInterval()
{
	AddCurrentHunger(-(PlayerStatus.HungerDecreaseAmount));
	
	if (PlayerStatus.Hunger<=KINDA_SMALL_NUMBER)
	{
		if (!bIsStarving)
		{
			bIsStarving=true;
			ApplyHungerDebuff();
			OnHungerDebuffChanged.Broadcast(true);
		}
	}
	else
	{
		if (bIsStarving)
		{
			bIsStarving=false;
			RemoveHungerDebuff();
			OnHungerDebuffChanged.Broadcast(false);
		}
	}
}

void UPlayerCharacterStatusComponent::StartSprint()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld())) return;
	
	if (CR4S_ENSURE(LogHong1,GetCurrentResource()>=PlayerStatus.SprintResourceCost))
	{
		GetWorld()->GetTimerManager().SetTimer(
			SprintTimerHandle,
			this,
			&UPlayerCharacterStatusComponent::ConsumeResourceForSprint,
			PlayerStatus.SprintCostConsumptionInterval,
			true
		);
	}
}

void UPlayerCharacterStatusComponent::StopSprint()
{
	if (!CR4S_ENSURE(LogHong1,GetWorld()&&OwningCharacter)) return;

	OwningCharacter->SetDesiredGait(AlsGaitTags::Running);
	GetWorld()->GetTimerManager().ClearTimer(SprintTimerHandle);
}

void UPlayerCharacterStatusComponent::ConsumeResourceForSprint()
{
	if (CR4S_ENSURE(LogHong1,GetCurrentResource()<PlayerStatus.SprintResourceCost))
	{
		StopSprint();
		return;
	}
	AddCurrentResource(-(PlayerStatus.SprintResourceCost));
	OnResourceConsumed();
}

void UPlayerCharacterStatusComponent::AddMaxHunger(const float InAmount)
{
	PlayerStatus.MaxHunger+=InAmount;
	const float Percentage=FMath::Clamp((PlayerStatus.Hunger)/PlayerStatus.MaxHunger,0.f,1.f);
	OnHungerChanged.Broadcast(Percentage);
}

void UPlayerCharacterStatusComponent::AddCurrentHunger(const float InAmount)
{
	const float Temp=FMath::Clamp(PlayerStatus.Hunger+InAmount,0,PlayerStatus.MaxHunger);
	PlayerStatus.Hunger=Temp;
	const float Percentage=FMath::Clamp((PlayerStatus.Hunger)/PlayerStatus.MaxHunger,0.f,1.f);
	OnHungerChanged.Broadcast(Percentage);
}


