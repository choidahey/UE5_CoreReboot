// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatStatics.h"

#include "CR4S.h"
#include "StunnableInterface.h"

void UCombatStatics::ApplyStun(AActor* Target, float StunAmount)
{
	if (!CR4S_ENSURE(LogHong1,Target&&(StunAmount>=0))) return;

	if (Target->GetClass()->ImplementsInterface(UStunnableInterface::StaticClass()))
	{
		IStunnableInterface::Execute_TakeStun(Target,StunAmount);
	}
}
