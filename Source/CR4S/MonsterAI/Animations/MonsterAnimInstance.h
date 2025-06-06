#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MonsterAnimInstance.generated.h"

UCLASS()
class CR4S_API UMonsterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	APawn* OwningPawn;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator ControlRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FRotator ActorRotation;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, Category = "AimOffset")
	float AimPitch;

};
