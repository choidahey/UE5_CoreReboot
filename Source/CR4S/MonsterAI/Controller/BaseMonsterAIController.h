#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;

UCLASS()
class CR4S_API ABaseMonsterAIController : public AAIController
{
	GENERATED_BODY()

#pragma region AIController Override
public:
	ABaseMonsterAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

#pragma endregion

#pragma region AIController Setting
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	UBehaviorTreeComponent* BehaviorTreeComp;

#pragma endregion
	
};
