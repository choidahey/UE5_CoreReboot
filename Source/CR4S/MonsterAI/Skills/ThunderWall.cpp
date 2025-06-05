// Fill out your copyright notice in the Description page of Project Settings.


#include "ThunderWall.h"


// Sets default values
AThunderWall::AThunderWall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AThunderWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThunderWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

