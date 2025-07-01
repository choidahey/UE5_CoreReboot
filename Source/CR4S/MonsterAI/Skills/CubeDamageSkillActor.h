#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "CubeDamageSkillActor.generated.h"

UCLASS()
class CR4S_API ACubeDamageSkillActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	ACubeDamageSkillActor();

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

protected:
	UPROPERTY(EditAnywhere, Category = "CubeDamage")
	FVector BoxExtent = FVector(200.f, 200.f, 100.f);

	UPROPERTY(EditAnywhere, Category = "CubeDamage")
	float LifeTime = 1.5f;

};
