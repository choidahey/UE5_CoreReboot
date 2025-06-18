// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTrace.h"

#include "Character/Components/WeaponTraceComponent.h"
#include "Character/Components/InputBufferComponent.h"
#include "Character/Components/RobotWeaponComponent.h"
#include "GameFramework/Character.h"

void UWeaponTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                               const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	if (UWeaponTraceComponent* WeaponTraceComp=OwningCharacter->FindComponentByClass<UWeaponTraceComponent>())
	{
		WeaponTraceComp->SetWeaponTrace(true);
	}
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UWeaponTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	if (UWeaponTraceComponent* WeaponTraceComp=OwningCharacter->FindComponentByClass<UWeaponTraceComponent>())
	{
		WeaponTraceComp->SetWeaponTrace(false);
	}
	//Super::NotifyEnd(MeshComp, Animation, EventReference);
}
