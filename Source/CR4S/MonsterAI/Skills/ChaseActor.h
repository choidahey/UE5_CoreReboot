#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "ChaseActor.generated.h"


UCLASS()
class CR4S_API AChaseSkillActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AChaseSkillActor();

	void Initialize();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase")
	float DelayTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase", meta=(ClampMin="0.0"))
	float Lifetime = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase", meta=(ClampMin="0.0"))
	float ChaseSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase", meta=(ClampMin="0.0"))
	float SpawnDistance = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase")
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill|Chase")
	float ZAdjustSpeed = 300.f;
	
	UPROPERTY(EditAnywhere, Category="Skill|Chase")
	bool bEnableMeshRotation = false;
	
	UPROPERTY(EditAnywhere, Category="Skill|Chase", meta=(EditCondition="bEnableMeshRotation"))
	FVector RotationAxis = FVector(0,0,1);

	UPROPERTY(EditAnywhere, Category="Skill|Chase", meta=(EditCondition="bEnableMeshRotation"))
	float RotationSpeed = 180.f;

	UPROPERTY(EditAnywhere, Category="Skill|Chase")
	bool bEnableGroundEffectSpawn = false;

	UPROPERTY(EditAnywhere, Category="Skill|Chase", meta=(ClampMin="0.01"), meta=(EditCondition="bEnableGroundEffectSpawn"))
	float MovementSpawnInterval = 0.2f;
	
	UPROPERTY(EditAnywhere, Category="Skill|Chase", meta=(EditCondition="bEnableGroundEffectSpawn"))
	TSubclassOf<AActor> MovementSpawnClass;


private:
	void ActivateChase();

	UPROPERTY()
	APawn* Target = nullptr;
	float MovementSpawnTimer = 0.f;
	bool bIsChasing = false;
	bool bLockZ = false;
	FTimerHandle DelayTimerHandle;
};

