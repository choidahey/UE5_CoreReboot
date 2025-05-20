// Fill out your copyright notice in the Description page of Project Settings.


#include "InputDisabled.h"
#include "Character/Components/CombatComponent.h"
#include "GameFramework/Character.h"

void UInputDisabled::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                 const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	UCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCombatComponent>();
	if (!Combat) return;

	Combat->SetInputEnable(false);
	
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UInputDisabled::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	UCombatComponent* Combat=OwningCharacter->FindComponentByClass<UCombatComponent>();
	if (!Combat) return;

	Combat->SetInputEnable(true);
	
	//Super::NotifyEnd(MeshComp, Animation, EventReference);
}
