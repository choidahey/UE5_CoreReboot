#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimalAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class ABaseAnimal;

UCLASS()
class CR4S_API UAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	ABaseAnimal* OwnerAnimal;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovementComponent;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	UFUNCTION()
	void AnimNotify_AnimalAttackHit();

	UFUNCTION()
	void AnimNotify_AnimalDash();

	UFUNCTION()
	void AnimNotify_AnimalRanged();
};
