#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HelperBotAnimInstance.generated.h"

UCLASS()
class CR4S_API UHelperBotAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

#pragma region Native
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
#pragma endregion

#pragma region Variables - References
	
protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ABaseHelperBot> OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ABaseAnimal> OwnerAnimal = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UCharacterMovementComponent> OwnerCharacterMovementComponent = nullptr;
	
#pragma endregion

#pragma region Variables - Movement

protected:
	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsJumping = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsWorking = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling = false;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FVector HorizontalVelocity = FVector::ZeroVector;
	
#pragma endregion
};
