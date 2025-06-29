#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "SphereDamageSkillActor.generated.h"

UCLASS()
class CR4S_API ASphereDamageSkillActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	ASphereDamageSkillActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	void SetupCollisionComponent();
	void SnapToGroundIfNeeded();
	void ActivateEffect();
	void ApplyInitialOverlapDamage();
	bool IsActorOnGround(AActor* Actor) const;

protected:
	UPROPERTY(EditAnywhere, Category = "Skill")
	float SphereRadius = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float LifeTime = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Skill|Damage")
	bool bAffectOnlyGroundTargets = true;

	UPROPERTY(EditAnywhere, Category = "Skill|FX")
	bool bSnapToGround = true;

	UPROPERTY(EditAnywhere, Category = "Skill|FX", meta = (EditCondition = "bSnapToGround"))
	float GroundOffsetZ = 100.f;

	UPROPERTY(EditAnywhere, Category = "Skill|FX", meta = (EditCondition = "bSnapToGround"))
	float GroundTraceDistance = 2000.f;

};
