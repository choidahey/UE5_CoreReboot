// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Character/Characters/PlayerCharacter.h"


// Sets default values for this component's properties
UCombatComponent::UCombatComponent():
	bInputEnable(true)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UCombatComponent::Input_OnAttack()
{
	if (!bInputEnable) return;
	OwningCharacter->PlayAnimMontage(AttackMontage);
}

void UCombatComponent::SetInputEnable(bool Enable)
{
	bInputEnable=Enable;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

