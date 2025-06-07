// Fill out your copyright notice in the Description page of Project Settings.


#include "InputDisabled.h"

#include "Character/Components/CharacterCombatComponent.h"
#include "Character/Components/CombatComponent.h"
#include "Character/Components/RobotCombatComponent.h"
#include "GameFramework/Character.h"

void UInputDisabled::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                 const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;
	
	if (UCharacterCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCharacterCombatComponent>())
	{
		Combat->SetInputEnable(false);
	}
	else if (URobotCombatComponent* RobotCombat=OwningCharacter->FindComponentByClass<URobotCombatComponent>())
	{
		RobotCombat->SetInputEnable(false);
	}
	
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UInputDisabled::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;
	
	if (UCharacterCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCharacterCombatComponent>())
	{
		Combat->SetInputEnable(true);
	}
	else if (URobotCombatComponent* RobotCombat=OwningCharacter->FindComponentByClass<URobotCombatComponent>())
	{
		RobotCombat->SetInputEnable(true);
	}
	
	//Super::NotifyEnd(MeshComp, Animation, EventReference);
}
