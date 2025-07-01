#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FlyingAnimalAnimInstance.generated.h"

class AAnimalFlying;

UCLASS()
class CR4S_API UFlyingAnimalAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFlyingAnimalAnimInstance();

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY()
	AAnimalFlying* OwningAnimal;

	// State Transition Parameters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool AreWeWalking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool AreWeFlying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool PlayIdleLoop = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool IsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool PlayTakeOffAnimation = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool FlyToPerch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool LandOnPerch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool AreWePerched = false;

	// Movement Parameters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Direction = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float WalkingSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	int32 RandomIdleAnimation = 0;

	// Flying Parameters
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flying")
	bool FlapFast = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flying")
	bool FlyingDownward = false;

private:
	void UpdateMovementParameters();
	void UpdateStateParameters();
	void UpdateFlyingParameters();
};