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
	void InitialLaunch(AActor* InTarget, int32 InIndex, int32 TotalCount);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	UProjectileMovementComponent* ProjectileMovementComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	bool bSequentialLaunch = true;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack", meta=(ClampMin="0.0"))
	float Interval = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Speed = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float MaxSpeed = 1500.f;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

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

	void Launch();
	void HandleSequenceLaunch();
	void HandleImmediateLaunch() const;
	
	bool bHasLaunched = false;
	int32 SpawnOrder = 0;
	int32 TotalCount = 0;
	float LaunchDelay = 0.f;

	FTimerHandle LaunchTimerHandle;
	FTimerHandle DestroyTimerHandle;
	
	FString MyHeader = TEXT("ColdFairyActor");
};
