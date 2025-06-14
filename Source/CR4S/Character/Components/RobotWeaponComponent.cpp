// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotWeaponComponent.h"
#include "CR4S.h"
#include "InputBufferComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/BaseWeapon.h"
#include "Character/Weapon/MeleeWeapon.h"
#include "Character/Weapon/RangedWeapon.h"
#include "Utility/DataLoaderSubsystem.h"


// Sets default values for this component's properties
URobotWeaponComponent::URobotWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Weapons.Init(nullptr,4);
}

void URobotWeaponComponent::Input_OnAttackLeftArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;
	FGameplayTag TempTag=Weapons[0]->GetGameplayTag();
	if ((TempTag.MatchesTag(WeaponTags::Melee)&&InputBuffer->CheckInputQueue(EInputType::RobotAttack1))
		||TempTag.MatchesTag(WeaponTags::Ranged))
	{
		ActivatedWeaponIdx=0;
		Weapons[0]->OnAttack();
	}
}

void URobotWeaponComponent::Input_OnAttackRightArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;
	FGameplayTag TempTag=Weapons[0]->GetGameplayTag();
	if ((TempTag.MatchesTag(WeaponTags::Melee)&&InputBuffer->CheckInputQueue(EInputType::RobotAttack2))
		||TempTag.MatchesTag(WeaponTags::Ranged))
	{
		ActivatedWeaponIdx=1;
		Weapons[1]->OnAttack();
	}
}

void URobotWeaponComponent::Input_OnAttackLeftShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (Weapons.IsValidIndex(2)&&IsValid(Weapons[2]))
	{
		Weapons[2]->OnAttack();
	}
}

void URobotWeaponComponent::Input_OnAttackRightShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (Weapons.IsValidIndex(3)&&IsValid(Weapons[3]))
	{
		Weapons[3]->OnAttack();
	}
}

void URobotWeaponComponent::EquipWeaponByTag(const FGameplayTag& Tag, const int32 SlotIdx)
{
	ABaseWeapon* NewWeapon=nullptr;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator=Cast<ACharacter>(GetOwner());
	SpawnParams.Owner=Cast<ACharacter>(GetOwner());
	if (Tag.MatchesTag(WeaponTags::Ranged))
	{
		NewWeapon=GetWorld()->SpawnActor<ARangedWeapon>(SpawnParams);
	}
	else if (Tag.MatchesTag(WeaponTags::Melee))
	{
		NewWeapon=GetWorld()->SpawnActor<AMeleeWeapon>(SpawnParams);
	}
	NewWeapon->SetGameplayTag(Tag);
	NewWeapon->Initialize(OwningCharacter);
	Weapons[SlotIdx]=NewWeapon;
}

void URobotWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter=Cast<AModularRobot>(GetOwner());
	for (int32 i=0;i<Weapons.Num();i++)
	{
		if (Weapons.IsValidIndex(i)&&IsValid(Weapons[i]))
		{
			Weapons[i]->Initialize(OwningCharacter);
		}
	}

	InputBuffer=OwningCharacter->FindComponentByClass<UInputBufferComponent>();
	if (!CR4S_ENSURE(LogHong1,InputBuffer)) return;

	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!CR4S_ENSURE(LogHong1,GI)) return;
	
	UDataLoaderSubsystem* Loader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!CR4S_ENSURE(LogHong1,Loader)) return;

	Loader->LoadWeaponSettingsData(WeaponSettings);
}


// Called every frame
void URobotWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
