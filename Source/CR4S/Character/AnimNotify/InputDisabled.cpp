// Fill out your copyright notice in the Description page of Project Settings.


#include "InputDisabled.h"

#include "CR4S.h"
#include "Character/Components/WeaponTraceComponent.h"
#include "Character/Components/InputBufferComponent.h"
#include "Character/Components/PlayerInputBufferComponent.h"
#include "Character/Components/RobotInputBufferComponent.h"
#include "Character/Components/RobotWeaponComponent.h"
#include "GameFramework/Character.h"

void UInputDisabled::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                 const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;

	if (UPlayerInputBufferComponent* Buffer=OwningCharacter->FindComponentByClass<UPlayerInputBufferComponent>())
	{
		UE_LOG(LogTemp, Display, TEXT("NotifyBegin got Buffer class: %s, name: %s, Enable : %d"),
		*Buffer->GetClass()->GetName(),
		*Buffer->GetName(), false);
		Buffer->SetInputEnable(false);
	}
	else if (URobotInputBufferComponent* RobotBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>())
	{
		RobotBuffer->SetInputEnable(false);
	}
	
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UInputDisabled::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	ACharacter* OwningCharacter=Cast<ACharacter>(MeshComp->GetOwner());
	if (!OwningCharacter) return;
	
	if (UPlayerInputBufferComponent* Buffer=OwningCharacter->FindComponentByClass<UPlayerInputBufferComponent>())
	{
		Buffer->SetInputEnable(true);
	}
	else if (URobotInputBufferComponent* RobotBuffer=OwningCharacter->FindComponentByClass<URobotInputBufferComponent>())
	{
		RobotBuffer->SetInputEnable(true);
	}
	
	//Super::NotifyEnd(MeshComp, Animation, EventReference);
}
