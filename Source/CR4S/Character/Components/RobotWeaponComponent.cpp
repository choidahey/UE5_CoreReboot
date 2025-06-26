// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotWeaponComponent.h"
#include "CR4S.h"
#include "InputBufferComponent.h"
#include "RobotInputBufferComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/RobotWeapon/BaseWeapon.h"
#include "Character/Weapon/RobotWeapon/HomingWeapon.h"
#include "Character/Weapon/RobotWeapon/RangedWeapon.h"
#include "Game/SaveGame/BaseSaveData.h"
#include "Game/SaveGame/ModularRobotSaveData.h"
#include "UI/InGame/SurvivalHUD.h"
#include "Utility/DataLoaderSubsystem.h"


// Sets default values for this component's properties
URobotWeaponComponent::URobotWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Weapons.Init(nullptr,4);
	WeaponSettings.AttachSocketNames.Init(FName(),4);
}

void URobotWeaponComponent::GatherWeaponSaveData(TArray<FRobotWeaponSaveGame>& OutWeaponData) const
{
	OutWeaponData.Empty();
	for (ABaseWeapon* Weapon:Weapons)
	{
		if (IsValid(Weapon))
		{
			OutWeaponData.Add(FRobotWeaponSaveGame(Weapon->GetToolGameplayTag(),Weapon->GetCurrentAmmo()));
		}
		else
		{
			OutWeaponData.Add(FRobotWeaponSaveGame());
		}
	}
}

void URobotWeaponComponent::ApplyWeaponSaveData(TArray<FRobotWeaponSaveGame>& InWeaponData)
{
	for (int32 i=0;i<InWeaponData.Num();i++)
	{
		const FRobotWeaponSaveGame& WeaponData = InWeaponData[i];

		if (WeaponData.WeaponTag.IsValid())
		{
			EquipWeaponByTag(WeaponData.WeaponTag,i);
			if (Weapons.IsValidIndex(i) && IsValid(Weapons[i]))
			{
				Weapons[i]->SetCurrentAmmo(WeaponData.CurrentAmmo);
			}
		}
	}
}


void URobotWeaponComponent::Input_OnAttackLeftArm()
{
	TryAttackBySlot(0,EInputType::RobotAttack1);
}

void URobotWeaponComponent::Input_StopAttackLeftArm()
{
	StopAttackBySlot(0);
}

void URobotWeaponComponent::Input_OnAttackRightArm()
{
	TryAttackBySlot(1,EInputType::RobotAttack2);
}

void URobotWeaponComponent::Input_StopAttackRightArm()
{
	StopAttackBySlot(1);
}

void URobotWeaponComponent::Input_OnAttackLeftShoulder()
{
	TryAttackBySlot(2,EInputType::RobotAttack3);
}

void URobotWeaponComponent::Input_StopAttackLeftShoulder()
{
	StopAttackBySlot(2);
}

void URobotWeaponComponent::Input_OnAttackRightShoulder()
{
	TryAttackBySlot(3,EInputType::RobotAttack4);
}

void URobotWeaponComponent::Input_StopAttackRightShoulder()
{
	StopAttackBySlot(3);
}

void URobotWeaponComponent::TryAttackBySlot(const int32 SlotIdx, const EInputType AttackInput)
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(SlotIdx)||!IsValid(Weapons[SlotIdx])) return;
	
	if (InputBuffer->CheckInputQueue(AttackInput))
	{
		Weapons[SlotIdx]->OnAttack();
	}
}

void URobotWeaponComponent::StopAttackBySlot(const int32 SlotIdx)
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(SlotIdx)||!IsValid(Weapons[SlotIdx])) return;

	//InputBuffer->ClearInputQueue();
	Weapons[SlotIdx]->StopAttack();
}

void URobotWeaponComponent::RefreshWeaponUI()
{
	for (ABaseWeapon* Weapon:Weapons)
	{
		if (IsValid(Weapon))
		{
			if (ARangedWeapon* RangedWeapon=Cast<ARangedWeapon>(Weapon))
			{
				RangedWeapon->RefreshUI();
			}
		}
	}
}

void URobotWeaponComponent::EquipWeaponByTag(const FGameplayTag& Tag, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,Weapons.IsValidIndex(SlotIdx) && OwningCharacter)) return;

	// Can't equip MeleeWeapon on Shouler (Only Arm)
	if (Tag.MatchesTag(WeaponTags::Melee) && !IsArmSlot(SlotIdx)) return;
	
	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!CR4S_ENSURE(LogHong1,GI)) return;
	
	UDataLoaderSubsystem* Loader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!CR4S_ENSURE(LogHong1,Loader)) return;

	TSubclassOf<ABaseWeapon> WeaponClass = nullptr;
	Loader->LoadWeaponClassDataByTag(Tag,WeaponClass);
	if (!CR4S_ENSURE(LogHong1,WeaponClass)) return;

	if (IsValid(Weapons[SlotIdx]))
	{
		UnequipWeapon(SlotIdx);
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator=OwningCharacter;
	SpawnParams.Owner=OwningCharacter;

	ABaseWeapon* NewWeapon=GetWorld()->SpawnActor<ABaseWeapon>(WeaponClass,SpawnParams);

	if (NewWeapon)
	{
		if (!WeaponSettings.AttachSocketNames.IsValidIndex(SlotIdx)||!WeaponSettings.AttachSocketNames[SlotIdx].IsValid())
		{
			NewWeapon->Destroy();
			return;
		}
		const FName AttachSocketName=WeaponSettings.AttachSocketNames[SlotIdx];

		NewWeapon->AttachToComponent(
			OwningCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			AttachSocketName
		);
		
		NewWeapon->SetGameplayTag(Tag);
		NewWeapon->Initialize(OwningCharacter, SlotIdx);
		Weapons[SlotIdx]=NewWeapon;

		UModularRobotStatusComponent* Status=OwningCharacter->GetStatusComponent();
		if (!CR4S_ENSURE(LogHong1,Status)) return;

		const float WeaponWeight=NewWeapon->GetWeaponWeight();
		Status->AddCurrentWeight(WeaponWeight);
		Status->AddCurrentArmMountWeight(WeaponWeight);
	}
}

void URobotWeaponComponent::UnequipWeapon(const int32 SlotIdx)
{
	if (!Weapons.IsValidIndex(SlotIdx) || !IsValid(Weapons[SlotIdx]) || !OwningCharacter) return;

	UModularRobotStatusComponent* Status=OwningCharacter->GetStatusComponent();
	if (!CR4S_ENSURE(LogHong1,Status)) return;

	const float WeaponWeight=Weapons[SlotIdx]->GetWeaponWeight();
	Status->AddCurrentWeight(-(WeaponWeight));
	Status->AddCurrentArmMountWeight(-(WeaponWeight));
	Weapons[SlotIdx]->Destroy();
	Weapons[SlotIdx]=nullptr;
}

void URobotWeaponComponent::UnequipAllWeapons()
{
	for (int32 i=0;i<Weapons.Num();i++)
	{
		UnequipWeapon(i);
	}
}

void URobotWeaponComponent::BindWidgetWeapon()
{
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!CR4S_ENSURE(LogHong1,PC)) return;

	ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(PC->GetHUD());
	if (!CR4S_ENSURE(LogHong1,CurrentHUD)) return;

	UDefaultInGameWidget* InGameWidget=Cast<UDefaultInGameWidget>(CurrentHUD->GetInGameWidget());
	if (!CR4S_ENSURE(LogHong1,InGameWidget)) return;

	for (int32 i=0;i<Weapons.Num();++i)
	{
		if (Weapons.IsValidIndex(i)&&IsValid(Weapons[i]))
		{
			InGameWidget->BindAmmoWidgetToWeapon(Weapons[i],i);
			if (AHomingWeapon* Homing=Cast<AHomingWeapon>(Weapons[i]))
			{
				InGameWidget->BindLockOnWidgetToHomingWeapon(Homing,i);
			}
		}
	}
}

bool URobotWeaponComponent::IsArmSlot(const int32 SlotIdx) const
{
	return SlotIdx==0 || SlotIdx==1;
}

void URobotWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter=Cast<AModularRobot>(GetOwner());
	for (int32 i=0;i<Weapons.Num();i++)
	{
		if (Weapons.IsValidIndex(i)&&IsValid(Weapons[i]))
		{
			Weapons[i]->Initialize(OwningCharacter, i);
		}
	}

	InputBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>();
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
