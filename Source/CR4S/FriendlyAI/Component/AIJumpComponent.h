#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkCustomComponent.h"
#include "AIJumpComponent.generated.h"

class UNavLinkCustomComponent;

DECLARE_DELEGATE(FAIJumpFailedSignature)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAIJumpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// UAIJumpComponent();
	//
	// FAIJumpFailedSignature OnJumpFailed;
	//
// 	UFUNCTION(BlueprintCallable)
// 	void TryJump();
//
// 	UFUNCTION()
// 	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
//
// 	virtual void Activate(bool bReset = false) override;
// 	virtual void Deactivate() override;
//
// 	UFUNCTION()
// 	void SetJumpPower(float NewPower) { JumpPower = NewPower; CurrentJumpPower = NewPower; }
// 	
// protected:
// 	virtual void BeginPlay() override;
// 	
// 	UPROPERTY(EditAnywhere, Category="EQS")
// 	UEnvQuery* JumpEQSQuery;
//
// 	UPROPERTY(EditAnywhere, Category="EQS")
// 	FName BlackboardTargetKeyName = TEXT("TargetActor");
//
// private:
// 	UFUNCTION()
// 	void OnCustomLinkReached(AActor* Agent, const FVector Destination);
// 	
// 	UFUNCTION()
// 	void RunEQSQuery();
//
// 	UFUNCTION()
// 	void OnCharacterLanded(const FHitResult& Hit);
// 	
// 	void StartEQSTimer();
// 	
// private:
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump", meta=(AllowPrivateAccess="true"))
// 	float JumpPower = 0.f;
//
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Jump", meta=(AllowPrivateAccess="true"))
// 	float CurrentJumpPower = 0.f;
// 	
// 	class ACharacter* OwnerCharacter = nullptr;
// 	
// 	FVector PendingLaunchVelocity = FVector::ZeroVector;
// 	TWeakObjectPtr<ANavLinkProxy> CurrentLinkProxy;
// 	
// 	FTimerHandle EQSTimerHandle;
//
// private:
// 	UFUNCTION()
// 	void OnNavMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);	
};