#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterAIController.generated.h"

class ABaseMonster;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class UBehaviorTree;
class UMonsterPerceptionComponent;

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
	void SetupPerceptionFromMonster(ABaseMonster* Monster);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UBlackboardComponent> BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<UMonsterPerceptionComponent> PerceptionComp;

#pragma endregion

#pragma region Chasing
public:
	UFUNCTION(BlueprintCallable, Category = "Monster|AI")
	void StartChasingPlayer(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Monster|AI")
	void StopChasingPlayer();

	UFUNCTION(BlueprintCallable, Category = "Monster|AI")
	void StartSearchingLastLocation();

#pragma endregion

#pragma region Monster Event Handler

protected:
	/* Called when target is detected via perception */
	UFUNCTION()
	virtual void OnTargetDetected(AActor* DetectedActor);

	/* Called when target is lost via perception */
	UFUNCTION()
	virtual void OnTargetLost(AActor* LostActor);

#pragma endregion

private:
	FString MyHeader;

};
