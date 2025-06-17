#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSkillActor.generated.h"

class UPrimitiveComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class CR4S_API ABaseSkillActor : public AActor
{
	GENERATED_BODY()
	
#pragma region Constructor & Overrides

public:
	ABaseSkillActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	);

public:
	virtual void InitializeSkillData();
	virtual void ApplyInitialOverlapDamage();
	virtual void ApplyEffectToActor(AActor* Target);

#pragma endregion

#pragma region Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill|Base")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill|Base")
	TObjectPtr<UPrimitiveComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill|Base")
	TObjectPtr<UStaticMeshComponent> StaticMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Skill|Base")
	TObjectPtr<UNiagaraComponent> NiagaraComp = nullptr;

#pragma endregion

#pragma region FX / SFX

protected:
	void PlayEffectAtLocation(const FVector& Location);
	void PlaySoundAtLocation(const FVector& Location);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|FX")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|FX")
	TObjectPtr<USoundBase> HitSound;

#pragma endregion

#pragma region Skill Attributes

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Attributes")
	float Damage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Attributes")
	float StunGaugeAmount = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Attributes")
	bool bAllowMultipleHits = false;

	UPROPERTY()
	TSet<TWeakObjectPtr<AActor>> AlreadyDamaged;

#pragma endregion


};

