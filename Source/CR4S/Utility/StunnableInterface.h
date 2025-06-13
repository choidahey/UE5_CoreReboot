// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StunnableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UStunnableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CR4S_API IStunnableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Stun")
	void TakeStun(const float StunAmount);
};
