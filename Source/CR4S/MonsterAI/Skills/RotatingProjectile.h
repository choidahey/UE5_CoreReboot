#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "Utility/NiagaraParamHelper.h"
#include "RotatingProjectile.generated.h"

class UCapsuleComponent;

UCLASS()
class CR4S_API ARotatingProjectile : public ABaseSkillActor
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

protected:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult
	) override;

	UFUNCTION()
	void OnLandingDetected(
		UPrimitiveComponent* OverlappedComp,
		AActor* Other, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	FVector ComputeParabolicVelocity(const FVector& Start, const FVector& Target, float Speed) const;
	void UpdateParabolicMovement(float DeltaTime);
	void HandleLanding();
	void UpdateLandingCollision();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Transform")
	FVector ProjectileScale = FVector(1.0f);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "VisualWeapon")
	TSubclassOf<AActor> VisualWeaponClass;

	UPROPERTY(VisibleAnywhere, Category = "Projectile|Collision")
	TObjectPtr<UCapsuleComponent> LandingTrigger;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Movement")
	float RotatingSpeed = 360.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Movement")
	bool bIsParabolic = false;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Movement")
	float GravityZ = -1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Lifetime")
	bool bDestroyOnBossApproach = false;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Lifetime")
	float AutoDestroyDelay = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Lifetime")
	float BossApproachRadius = 200.0f;


private:
	TObjectPtr<AActor> BossActor;
	FName RestoreSocketName;

	bool bIsLaunched = false;
	bool bHasLanded = false;

	FVector TargetLocation;
	FVector InitialPosition;
	FVector InitialVelocity;
	float TimeSinceLaunch = 0.f;

	FVector MoveDirection;
	float MoveSpeed = 0.f;

	FString MyHeader;

#pragma region Trail Effect

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Trail")
	TObjectPtr<UNiagaraSystem> TrailEffect = nullptr;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailEffectComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile|Trail")
	FNiagaraParamSet TrailEffectParams;

private:
	void SpawnAndAttachTrailEffect();

#pragma endregion

#pragma region Landing Effect

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Projectile|Landing")
	TObjectPtr<UNiagaraSystem> LandingEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "Projectile|Landing")
	FVector LandingEffectOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Projectile|Landing")
	FNiagaraParamSet LandingEffectParams;

private:
	void PlayLandingEffect();

#pragma endregion

};
