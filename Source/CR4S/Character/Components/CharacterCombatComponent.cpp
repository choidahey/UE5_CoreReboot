// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterCombatComponent.h"

#include "CR4S.h"
#include "PlayerCharacterStatusComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UCharacterCombatComponent::UCharacterCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UCharacterCombatComponent::Input_OnAttack()
{
	if (!CheckInputQueue(EInputType::Attack)) return;
	UStaticMesh* ToolMesh=OwningCharacter->GetToolStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)||!CR4S_ENSURE(LogHong1,AttackMontage)) return;
	OwningCharacter->PlayAnimMontage(AttackMontage);
}

void UCharacterCombatComponent::PerformWeaponTrace()
{
	if (!bWeaponTrace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	UStaticMesh* ToolMesh=OwningCharacter->GetToolStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)) return;
	//Socket Location
	FVector CurrentTop=Weapon->GetSocketLocation(TopSocketName);
	FVector CurrentBottom=Weapon->GetSocketLocation(BottomSocketName);
	float Damage=0;
	if (UPlayerCharacterStatusComponent* StatusComp=OwningCharacter->FindComponentByClass<UPlayerCharacterStatusComponent>())
	{
		Damage=StatusComp->GetAttackPower();
	}
	SweepAndApplyDamage(OwningCharacter,CurrentTop,CurrentBottom,Damage);
}

void UCharacterCombatComponent::SetWeaponTrace(const bool Trace)
{
	AlreadyDamagedActors.Empty();
	bWeaponTrace=Trace;
	if (!Trace) return;
	UStaticMeshComponent* Weapon=OwningCharacter->GetOverlayStaticMesh();
	if (!Weapon) return;
	UStaticMesh* ToolMesh=Weapon->GetStaticMesh();
	if (!CR4S_ENSURE(LogHong1,ToolMesh)) return;
	PreviousTopLocation=Weapon->GetSocketLocation(TopSocketName);
	PreviousBottomLocation=Weapon->GetSocketLocation(BottomSocketName);
}

void UCharacterCombatComponent::ExecuteInputQueue()
{
	switch (CurrentInputQueue)
	{
	case EInputType::None:
		break;
	case EInputType::Attack:
		Input_OnAttack();
		break;
	default:
		break;
	}
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());
}


void UCharacterCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

