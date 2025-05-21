#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "AIJumpComponent.generated.h"

class UNavLinkCustomComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAIJumpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIJumpComponent();

	UFUNCTION(BlueprintCallable)
	void TryJump();

	UFUNCTION()
	void OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="EQS")
	UEnvQuery* JumpEQSQuery;

private:
	void StartEQSTimer();
	
	FTimerHandle EQSTimerHandle;

	class ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump", meta=(AllowPrivateAccess="true"))
	float JumpPower = 600.f;

	FVector PendingLaunchVelocity = FVector::ZeroVector;

	UFUNCTION()
	void OnCustomLinkReached(UNavLinkCustomComponent* SmartLinkComp, UObject* PathComp, const FVector& DestPoint);
	
	UFUNCTION()
	void RunEQSQuery();

	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);
};