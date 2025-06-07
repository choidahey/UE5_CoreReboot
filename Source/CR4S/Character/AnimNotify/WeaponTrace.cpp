// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTrace.h"

#include "Character/Components/CharacterCombatComponent.h"
#include "Character/Components/CombatComponent.h"
#include "Character/Components/RobotCombatComponent.h"
#include "GameFramework/Character.h"

void UWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                               const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	if (UCharacterCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCharacterCombatComponent>())
	{
		Combat->SetWeaponTrace(true);
	}
	else if (URobotCombatComponent* RobotCombat=OwningCharacter->FindComponentByClass<URobotCombatComponent>())
	{
		RobotCombat->SetWeaponTrace(true);
	}
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	if (UCharacterCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCharacterCombatComponent>())
	{
		Combat->SetWeaponTrace(false);
	}
	else if (URobotCombatComponent* RobotCombat=OwningCharacter->FindComponentByClass<URobotCombatComponent>())
	{
		RobotCombat->SetWeaponTrace(false);
	}
	//Super::NotifyEnd(MeshComp, Animation, EventReference);
}
