#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingProjectile.generated.h"

class UCapsuleComponent;

UCLASS()
class CR4S_API ARotatingProjectile : public AActor
{
	GENERATED_BODY()

public:
	ARotatingProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

public :
	void LaunchProjectile(const FVector& TargetLocation, float Speed);
	void SetBossActor(AActor* InBoss, FName InSocket);

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	FVector ProjectileScale = FVector(1.0f);

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnLandingDetected(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector ComputeParabolicVelocity(const FVector& Start, const FVector& Target, float Speed) const;
	void UpdateParabolicMovement(float DeltaTime);
	void HandleLanding();
	void UpdateLandingCollision();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	TObjectPtr<UCapsuleComponent> CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	TObjectPtr<UCapsuleComponent> LandingTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float RotatingSpeed = 360.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float GravityZ = -1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	bool bIsParabolic = false;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float AutoDestroyDelay = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	bool bDestroyOnBossApproach = false;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float BossApproachRadius = 200.0f;

private:
	TObjectPtr<AActor> BossActor;
	FName RestoreSocketName;

	TSet<AActor*> AlreadyHitActors;
	bool bIsLaunched = false;
	bool bHasLanded = false;

	FVector TargetLocation;
	FVector InitialPosition;
	FVector InitialVelocity;
	float TimeSinceLaunch = 0.f;

	FVector MoveDirection;
	float MoveSpeed = 0.f;

	FString MyHeader;

};
