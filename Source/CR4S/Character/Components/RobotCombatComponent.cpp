// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotCombatComponent.h"
#include "CR4S.h"
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
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;
	FGameplayTag TempTag=Weapons[0]->GetGameplayTag();
	if ((TempTag.MatchesTag(WeaponTags::Melee)&&CheckInputQueue(EInputType::RobotAttack1))
		||TempTag.MatchesTag(WeaponTags::Ranged))
	{
		ActivatedWeaponIdx=0;
		Weapons[0]->OnAttack();
	}
}

void URobotCombatComponent::Input_OnAttackRightArm()
{
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;
	FGameplayTag TempTag=Weapons[0]->GetGameplayTag();
	if ((TempTag.MatchesTag(WeaponTags::Melee)&&CheckInputQueue(EInputType::RobotAttack2))
		||TempTag.MatchesTag(WeaponTags::Ranged))
	{
		ActivatedWeaponIdx=1;
		Weapons[1]->OnAttack();
	}
}

void URobotCombatComponent::Input_OnAttackLeftShoulder()
{
	if (Weapons.IsValidIndex(2)&&IsValid(Weapons[2]))
	{
		Weapons[2]->OnAttack();
	}
}

void URobotCombatComponent::Input_OnAttackRightShoulder()
{
	if (Weapons.IsValidIndex(3)&&IsValid(Weapons[3]))
	{
		Weapons[3]->OnAttack();
	}
}

void URobotCombatComponent::EquipWeaponByTag(const FGameplayTag Tag, const int32 SlotIdx)
{
	UBaseWeapon* NewWeapon=nullptr;
	if (Tag.MatchesTag(WeaponTags::Ranged))
	{
		NewWeapon=NewObject<URangedWeapon>(this);
	}
	else if (Tag.MatchesTag(WeaponTags::Melee))
	{
		NewWeapon=NewObject<UMeleeWeapon>(this);
	}
	NewWeapon->SetGameplayTag(Tag);
	NewWeapon->Initialize(OwningCharacter);
	Weapons[SlotIdx]=NewWeapon;
}

void URobotCombatComponent::PerformWeaponTrace()
{
	if (!bWeaponTrace) return;
	USkeletalMeshComponent* SkMesh=OwningCharacter->GetMesh();
	if (!SkMesh) return;
	//Socket Location
	FVector CurrentTop=SkMesh->GetSocketLocation(TopSocketName);
	FVector CurrentBottom=SkMesh->GetSocketLocation(BottomSocketName);
	
	if (CR4S_ENSURE(LogHong1,!IsValid(Weapons[ActivatedWeaponIdx]))) return;
	
	const float Damage=Weapons[ActivatedWeaponIdx]->ComputeFinalDamage();
	SweepAndApplyDamage(OwningCharacter,CurrentTop,CurrentBottom,Damage);
}

void URobotCombatComponent::SetWeaponTrace(const bool Trace)
{
	AlreadyDamagedActors.Empty();
	bWeaponTrace=Trace;
	if (!Trace)
	{
		ActivatedWeaponIdx=-1;
		return;
	}
	USkeletalMeshComponent* SkMesh=OwningCharacter->GetMesh();
	if (!CR4S_ENSURE(LogHong1,SkMesh)) return;
	PreviousTopLocation=SkMesh->GetSocketLocation(TopSocketName);
	PreviousBottomLocation=SkMesh->GetSocketLocation(BottomSocketName);
}

void URobotCombatComponent::ExecuteInputQueue()
{
	switch (CurrentInputQueue)
	{
	case EInputType::None:
		break;
	case EInputType::RobotAttack1:
		Input_OnAttackLeftArm();
		break;
	case EInputType::RobotAttack2:
		Input_OnAttackRightArm();
	default:
		break;
	}
}

void URobotCombatComponent::BeginPlay()
{
	CR4S_SIMPLE_SCOPE_LOG;
	Super::BeginPlay();
	OwningCharacter=Cast<AModularRobot>(GetOwner());
	for (int32 i=0;i<Weapons.Num();i++)
	{
		if (Weapons.IsValidIndex(i)&&IsValid(Weapons[i]))
		{
			Weapons[i]->Initialize(OwningCharacter);
		}
	}
}


// Called every frame
void URobotCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
