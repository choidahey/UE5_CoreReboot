// Fill out your copyright notice in the Description page of Project Settings.


#include "InputBufferComponent.h"
#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Character/Weapon/PlayerTool.h"

// Sets default values for this component's properties
UInputBufferComponent::UInputBufferComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UInputBufferComponent::SetInputEnable(const bool Enable)
{
	bInputEnable=Enable;
	if (Enable)
	{
		ExecuteInputQueue();
	}
}

void UInputBufferComponent::SetInputQueue(const EInputType Input)
{
	CurrentInputQueue=Input;

	if (BufferClearTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(BufferClearTimerHandle);
	}
	GetWorld()->GetTimerManager().SetTimer(
		BufferClearTimerHandle,
		this,
		&UInputBufferComponent::ClearInputQueue,
		1.f,
		false
	);
}

bool UInputBufferComponent::CheckInputQueue(const EInputType Input)
{
	if (bInputEnable) return true;

	SetInputQueue(Input);
	return false;
}

void UInputBufferComponent::ExecuteInputQueue() const
{
}


// Called when the game starts
void UInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInputBufferComponent::ClearInputQueue()
{
	CurrentInputQueue=EInputType::None;
	BufferClearTimerHandle.Invalidate();
}


