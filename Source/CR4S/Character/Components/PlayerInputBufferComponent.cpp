// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInputBufferComponent.h"
#include "CR4S.h"
#include "Character/Weapon/PlayerTool.h"


// Sets default values for this component's properties
UPlayerInputBufferComponent::UPlayerInputBufferComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UPlayerInputBufferComponent::SetCurrentTool(APlayerTool* InTool)
{
	CurrentTool=InTool;
}


// Called when the game starts
void UPlayerInputBufferComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UPlayerInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UPlayerInputBufferComponent::ExecuteInputQueue() const
{
	switch (CurrentInputQueue)
	{
	case EInputType::None:
		break;
	case EInputType::Attack:
		if (!CR4S_ENSURE(LogHong1,CurrentTool)) return;
		CurrentTool->OnAttack();
		break;
	default:
		break;
	}
}

