#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Teleport.generated.h"

struct FPathFollowingResult;
class UNiagaraComponent;
class UNiagaraSystem;
class UMovementComponent;

UCLASS()
class CR4S_API UBTTaskNode_Teleport : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_Teleport();

	UPROPERTY(EditAnywhere, Category="Teleport")
	FBlackboardKeySelector TeleportLocationKey;

	UPROPERTY(EditAnywhere, Category="Teleport")
	TObjectPtr<UNiagaraSystem> TrailEffectTemplate = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Teleport")
	TObjectPtr<UNiagaraSystem> BodyEffectTemplate  = nullptr;
	
	UPROPERTY(EditAnywhere, Category="Teleport")
	FName AttachBoneName = TEXT("spine_01");
	
	UPROPERTY(EditAnywhere, Category="Teleport")
	float AcceptanceRadius = 20.f;
	
	UPROPERTY(EditAnywhere, Category="Teleport")
	float TeleportSpeed = 5000.f;
	
	UPROPERTY(EditAnywhere, Category="Teleport")
	float TeleportAcceleration = 99999.f;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	UPROPERTY()
	TObjectPtr<APawn> CachedPawn = nullptr;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedMeshComp = nullptr;
	
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailEffectComp = nullptr;
	UPROPERTY()
	TObjectPtr<UNiagaraComponent> BodyEffectComp  = nullptr;
	
	EMovementMode OriginalMovementMode = MOVE_Walking;
	FAIRequestID TeleportRequestID;
	
	float OriginalMaxSpeed = 600.f;
	float OriginalAcceleration = 1000.f;
};
