// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotWeaponComponent.h"
#include "CR4S.h"
#include "InputBufferComponent.h"
#include "RobotInputBufferComponent.h"
#include "Character/Characters/ModularRobot.h"
#include "Character/Weapon/RobotWeapon/BaseWeapon.h"
#include "Character/Weapon/RobotWeapon/RangedWeapon.h"
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

void URobotWeaponComponent::Input_OnAttackLeftArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;
	
	if (InputBuffer->CheckInputQueue(EInputType::RobotAttack1))
	{
		Weapons[0]->OnAttack();
	}
}

void URobotWeaponComponent::Input_StopAttackLeftArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(0)||!IsValid(Weapons[0])) return;

	//InputBuffer->ClearInputQueue();
	Weapons[0]->StopAttack();
}

void URobotWeaponComponent::Input_OnAttackRightArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(1)||!IsValid(Weapons[1])) return;
	
	if (InputBuffer->CheckInputQueue(EInputType::RobotAttack2))
	{
		Weapons[1]->OnAttack();
	}
}

void URobotWeaponComponent::Input_StopAttackRightArm()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(1)||!IsValid(Weapons[1])) return;

	//InputBuffer->ClearInputQueue();
	Weapons[1]->StopAttack();
}

void URobotWeaponComponent::Input_OnAttackLeftShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(2)||!IsValid(Weapons[2])) return;
	
	if (InputBuffer->CheckInputQueue(EInputType::RobotAttack3))
	{
		Weapons[2]->OnAttack();
	}
}

void URobotWeaponComponent::Input_StopAttackLeftShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(2)||!IsValid(Weapons[2])) return;

	//InputBuffer->ClearInputQueue();
	Weapons[2]->StopAttack();
}

void URobotWeaponComponent::Input_OnAttackRightShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	
	if (!Weapons.IsValidIndex(3)||!IsValid(Weapons[3])) return;
	
	if (InputBuffer->CheckInputQueue(EInputType::RobotAttack4))
	{
		Weapons[3]->OnAttack();
	}
}

void URobotWeaponComponent::Input_StopAttackRightShoulder()
{
	if (!OwningCharacter||!OwningCharacter->IsRobotActive()) return;
	if (!Weapons.IsValidIndex(3)||!IsValid(Weapons[3])) return;

	//InputBuffer->ClearInputQueue();
	Weapons[3]->StopAttack();
}

void URobotWeaponComponent::EquipWeaponByTag(const FGameplayTag& Tag, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,Weapons.IsValidIndex(SlotIdx))) return;

	// Can't equip MeleeWeapon on Shouler (Only Arm)
	if (Tag.MatchesTag(WeaponTags::Melee)&&SlotIdx>1) return;
	
	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!CR4S_ENSURE(LogHong1,GI)) return;
	
	UDataLoaderSubsystem* Loader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!CR4S_ENSURE(LogHong1,Loader)) return;

	TSubclassOf<ABaseWeapon> WeaponClass = nullptr;
	Loader->LoadWeaponClassDataByTag(Tag,WeaponClass);
	if (!CR4S_ENSURE(LogHong1,WeaponClass)) return;

	if (IsValid(Weapons[SlotIdx]))
	{
		Weapons[SlotIdx]->Destroy();
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
		NewWeapon->Initialize(OwningCharacter);
		Weapons[SlotIdx]=NewWeapon;
		BindWidgetWeapon(NewWeapon,SlotIdx);
	}
}

void URobotWeaponComponent::BindWidgetWeapon(ABaseWeapon* Target, const int32 SlotIdx)
{
	if (!CR4S_ENSURE(LogHong1,Target && OwningCharacter)) return;

	APlayerController* PC=Cast<APlayerController>(OwningCharacter->GetController());
	if (!CR4S_ENSURE(LogHong1,PC)) return;

	ASurvivalHUD* CurrentHUD=Cast<ASurvivalHUD>(PC->GetHUD());
	if (!CR4S_ENSURE(LogHong1,CurrentHUD)) return;

	UDefaultInGameWidget* InGameWidget=Cast<UDefaultInGameWidget>(CurrentHUD->GetInGameWidget());
	if (!CR4S_ENSURE(LogHong1,InGameWidget)) return;

	InGameWidget->BindAmmoWidgetToWeapon(Target,SlotIdx);
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
