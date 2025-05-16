// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterStatusComponent.h"

#include "Character/Characters/PlayerCharacter.h"


// Sets default values for this component's properties
UPlayerCharacterStatusComponent::UPlayerCharacterStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UPlayerCharacterStatusComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());

	if (StatusData)
	{
		Status=StatusData->StatsData;
	}
}


// Called every frame
void UPlayerCharacterStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

