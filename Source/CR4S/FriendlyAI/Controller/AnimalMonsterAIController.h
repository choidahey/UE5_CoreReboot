#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../BaseAnimal.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AnimalMonsterAIController.generated.h"

UCLASS()
class CR4S_API AAnimalMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAnimalMonsterAIController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	void SetAnimalState(EAnimalState NewState);
	void SetTargetActor(AActor* Target);
	void ClearTargetActor();
	void SetPlayerToBlackboard();
	
protected:
	UPROPERTY(VisibleAnywhere)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere)
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* MonsterBehaviorTree;
};
