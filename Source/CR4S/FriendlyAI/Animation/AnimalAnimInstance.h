#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimalAnimInstance.generated.h"

class ACharacter;
class UCharacterMovementComponent;
class ABaseAnimal;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnIdleMontageEnded, UAnimMontage*, Montage, bool, bInterrupted);

UCLASS()
class CR4S_API UAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void AnimNotify_AnimalAttackHit();

	UFUNCTION()
	void AnimNotify_AnimalDash();

	UFUNCTION()
	void AnimNotify_AnimalRanged();
	
protected:
	UPROPERTY()
	ABaseAnimal* OwnerAnimal;
	
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

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

#pragma region Idle RandomAnim
public:
	UFUNCTION(BlueprintCallable, Category="Idle")
	bool PlayRandomIdleMontage();

	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Idle")
	TArray<UAnimMontage*> IdleMontages;
	
	UPROPERTY(BlueprintAssignable, Category="Idle")
	FOnIdleMontageEnded OnIdleMontageEnded;
#pragma endregion
};
