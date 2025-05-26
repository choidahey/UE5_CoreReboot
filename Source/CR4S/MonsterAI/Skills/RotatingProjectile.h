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
	void LaunchProjectile(const FVector& Direction, float Speed);
	void SetBossActor(AActor* InBoss, FName InSocket);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	TObjectPtr<UCapsuleComponent> CollisionComp;

protected:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Attack")
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float RotatingSpeed = 360.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float AutoDestroyDelay = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	bool bDestroyOnBossApproach = false;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Attack")
	float BossApproachRadius = 200.0f;

private:
	void HandleLanding(AActor* HitActor);

	TObjectPtr<AActor> BossActor;
	FName RestoreSocketName;
	TSet<AActor*> AlreadyHitActors;
	
	FVector MoveDirection;
	float MoveSpeed = 0.f;
	bool bHasLanded = false;

	FString MyHeader;

};
