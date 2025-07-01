// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBufferComponent.h"
#include "PlayerInputBufferComponent.generated.h"


class APlayerTool;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlayerInputBufferComponent : public UInputBufferComponent
{
	GENERATED_BODY()

public:
	UPlayerInputBufferComponent();

public:
	virtual void SetCurrentTool(APlayerTool* InTool) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	virtual void ExecuteInputQueue() const override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerTool> CurrentTool;

};
