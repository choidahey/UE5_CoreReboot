#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blizzard.generated.h"

class ABaseMonster;
class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class CR4S_API ABlizzard : public AActor
{
	GENERATED_BODY()

public:
	ABlizzard();

	void Launch(ABaseMonster* BossMonster);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	USceneComponent* RootComp;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	UCapsuleComponent* OverlapComp;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	UNiagaraSystem* NiagaraTemplate;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float SpawnRadius = 4.f;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float SpawnHeight = 8.f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float Damage = 0.f;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float TickInterval = 0.2f;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float SpeedBuff = 1.5f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float LifeTime = 5.f;
	
	float Speed;

	UPROPERTY()
	ABaseMonster* OwnerMonster;
	
	FTimerHandle DamageTimerHandle;
	FTimerHandle LifetimeTimerHandle;
	
	void ApplyDamageTick();
	void EndSkill();

private:
	FString MyHeader;
};
