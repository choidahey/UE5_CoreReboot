#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FlyingAnimalAnimInstance.generated.h"


UCLASS()
class CR4S_API UFlyingAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bFlapFast : 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bFlyingDownward : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIdleLoop : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsWalking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsFlying : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsAir : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float WalkingSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RandomIdleAnim = 0;
};
