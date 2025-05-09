// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick/CompleteGimmick.h"
#include "DoorGimmick.generated.h"

UCLASS()
class CR4S_API ADoorGimmick : public ACompleteGimmick
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADoorGimmick();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
