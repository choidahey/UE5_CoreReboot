#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColdFairyActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class CR4S_API AColdFairyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AColdFairyActor();

	UFUNCTION(BlueprintCallable, Category = "Boss|Attack")
	void Launch(AActor* TargetActor);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Speed = 5000.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float MaxSpeed = 5000.f;

	FVector LaunchDirection;

private:
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
		);
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActorPtr = nullptr;

	FTimerHandle DisableTickTimerHandle;
	FTimerHandle DestroyTimerHandle;
	FString MyHeader = TEXT("ColdFairyActor");
};
