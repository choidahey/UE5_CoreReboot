// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UCombatComponent::SetInputEnable(const bool Enable)
{
	bInputEnable=Enable;
	if (Enable)
	{
		ExecuteInputQueue();
	}
}

void UCombatComponent::SetInputQueue(const EInputType Input)
{
	CurrentInputQueue=Input;

	if (BufferClearTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BufferClearTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(
		BufferClearTimerHandle,
		this,
		&UCombatComponent::ClearInputQueue,
		1.f,
		false
	);
}

bool UCombatComponent::CheckInputQueue(const EInputType Input)
{
	if (bInputEnable) return true;

	SetInputQueue(Input);
	return false;
}

void UCombatComponent::SetWeaponTrace(const bool Trace)
{
}

void UCombatComponent::ExecuteInputQueue()
{
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::ClearInputQueue()
{
	CurrentInputQueue=EInputType::None;
	BufferClearTimerHandle.Invalidate();
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
}

