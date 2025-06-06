#pragma once
#include "CoreMinimal.h"
#include "BaseAnimal.h"
#include "AnimalGround.generated.h"

class UAIJumpComponent;

UCLASS()
class CR4S_API AAnimalGround : public ABaseAnimal
{
	GENERATED_BODY()
	
public:
	AAnimalGround();
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UAIJumpComponent* AIJumpComponent;
	
#pragma region Stun
	
public:
	void ApplyStun(float Amount) override;
	void RecoverFromStun() override;
	
#pragma endregion

#pragma region Attack
	
	void PerformChargeAttack() override;
	
#pragma endregion
};
