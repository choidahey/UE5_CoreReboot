// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotCombatComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/BaseWeapon.h"
#include "Character/Weapon/MeleeWeapon.h"
#include "Character/Weapon/RangedWeapon.h"


// Sets default values for this component's properties
URobotCombatComponent::URobotCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Weapons.Init(nullptr,4);
}

void URobotCombatComponent::Input_OnAttackLeftArm()
{
	if (Weapons.IsValidIndex(0)&&IsValid(Weapons[0]))
	{
		Weapons[0]->OnAttack(0);
	}
}

void URobotCombatComponent::Input_OnAttackRightArm()
{
	if (Weapons.IsValidIndex(1)&&IsValid(Weapons[1]))
	{
		Weapons[1]->OnAttack(1);
	}
}

void URobotCombatComponent::Input_OnAttackLeftShoulder()
{
	if (Weapons.IsValidIndex(2)&&IsValid(Weapons[2]))
	{
		Weapons[2]->OnAttack(2);
	}
}

void URobotCombatComponent::Input_OnAttackRightShoulder()
{
	if (Weapons.IsValidIndex(3)&&IsValid(Weapons[3]))
	{
		Weapons[3]->OnAttack(3);
	}
}

void URobotCombatComponent::EquipWeaponByTag(FGameplayTag Tag, const int32 SlotIdx)
{
	UBaseWeapon* NewWeapon=nullptr;
	if (Tag.MatchesTag(WeaponTags::Ranged))
	{
		NewWeapon=NewObject<URangedWeapon>(OwningCharacter);
	}
	else if (Tag.MatchesTag(WeaponTags::Melee))
	{
		NewWeapon=NewObject<UMeleeWeapon>(OwningCharacter);
	}
	NewWeapon->SetGameplayTag(Tag);
	NewWeapon->Initialize(OwningCharacter);
	Weapons[SlotIdx]=NewWeapon;
}


// Called when the game starts
void URobotCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter=Cast<AModularRobot>(GetOwner());
	for (int32 i=0;i<Weapons.Num();i++)
	{
		if (Weapons.IsValidIndex(i)&&Weapons[i])
		{
			Weapons[i]->Initialize(OwningCharacter);
		}
	}
}


// Called every frame
void URobotCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
