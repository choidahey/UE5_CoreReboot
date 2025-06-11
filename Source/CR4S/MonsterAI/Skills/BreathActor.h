#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "BreathActor.generated.h"

class UNiagaraComponent;

UCLASS()
class CR4S_API ABreathActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:	
	ABreathActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void DoDamage();

public:
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void InitializeSkill(USkeletalMeshComponent* InMesh, FName InSocketName);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void ActivateSkill();

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void EndSkill();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Skill|Damage")
	float Range = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Damage")
	float Angle = 60.f;

	UPROPERTY(EditDefaultsOnly, Category = "Skill|Damage")
	float TickInterval = 0.2f;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> FollowMesh = nullptr;

	UPROPERTY()
	FName FollowSocketName = NAME_None;

private:
	FTransform LastSocketTransform;
	float TimeSinceLastTick = 0.f;
	bool bIsInitialized = false;
	bool bIsActivated = false;
};
