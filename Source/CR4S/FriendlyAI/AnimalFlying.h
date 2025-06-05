#pragma once

#include "CoreMinimal.h"
#include "BaseAnimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimalFlying.generated.h"

class UFlyingMovementComponent;

UCLASS()
class CR4S_API AAnimalFlying : public ABaseAnimal
{
	GENERATED_BODY()
	
public:
	AAnimalFlying();
	
public:
	virtual void BeginPlay() override;
	
#pragma region Stun
public:
	virtual void ApplyStun(float Amount) override;
	virtual void RecoverFromStun() override;
#pragma endregion
	
#pragma region Attack
	//void PerformMeleeAttack() override;
	void PerformChargeAttack() override;
	//void PerformRangedAttack() override;
#pragma endregion

public:
	virtual void Landed(const FHitResult& Hit) override;
	
private:
	UPROPERTY(VisibleAnywhere)
	UFlyingMovementComponent* FlyingComp;

private:
	UPROPERTY(EditAnywhere)
	float AltitudeChangeRate = 200.0f;

	UPROPERTY(EditAnywhere)
	float FlightSpeed = 600.0f;

	UPROPERTY(EditAnywhere)
	float MaxAltitude = 3000.0f;

	void LiftOff();
	void Land();
};
