#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "ProjectileShard.generated.h"

class UProjectileMovementComponent;

UCLASS()
class CR4S_API AProjectileShard : public ABaseSkillActor
{
	GENERATED_BODY()

public:
    AProjectileShard();

    void InitializeDirection(const FVector& InDirection, float InSpeed, float ExplosionRange);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    virtual void OnOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    ) override;

    void DestroyShard();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

    UPROPERTY(EditAnywhere, Category = "Config")
    float MaxTravelDistance = 3000.f;

private:
    FVector SpawnLocation;
};
