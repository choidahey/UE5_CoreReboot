#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
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

protected:
	virtual void BeginPlay() override;

private:
	class ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jump", meta=(AllowPrivateAccess="true"))
	float JumpPower = 600.f;

	FVector PendingLaunchVelocity = FVector::ZeroVector;

	UFUNCTION()
	void OnCustomLinkReached(UNavLinkCustomComponent* SmartLinkComp, UObject* PathComp, const FVector& DestPoint);
};
