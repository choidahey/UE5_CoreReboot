#pragma once
#include "CoreMinimal.h"
#include "BaseAnimal.h"
#include "AnimalMonster.generated.h"

class UAIJumpComponent;

UCLASS()
class CR4S_API AAnimalMonster : public ABaseAnimal
{
	GENERATED_BODY()

public:
	AAnimalMonster();
	virtual void BeginPlay() override;

#pragma region Stun
public:
	virtual void ApplyStun(float Amount) override;
	virtual void RecoverFromStun() override;
#pragma endregion

#pragma region Attack
public:
	virtual void PerformChargeAttack() override;
#pragma endregion
	
#pragma region Component
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	class UAIJumpComponent* AIJumpComponent;
#pragma endregion
	
};
