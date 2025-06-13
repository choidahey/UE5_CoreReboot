// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTool.h"


// Sets default values
ABaseTool::ABaseTool()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

AActor* ABaseTool::GetToolOwner() const
{
	return nullptr;
}

UMeshComponent* ABaseTool::GetToolMeshComponent()
{
	return nullptr;
}

FVector ABaseTool::GetSocketLocationByName(const FName SocketName)
{
	return FVector();
}

void ABaseTool::SetGameplayTag(const FGameplayTag GameplayTag)
{
	ToolTag=GameplayTag;
}

void ABaseTool::OnAttack()
{
}

float ABaseTool::ComputeFinalDamage()
{
	return 0;
}

// Called when the game starts or when spawned
void ABaseTool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

