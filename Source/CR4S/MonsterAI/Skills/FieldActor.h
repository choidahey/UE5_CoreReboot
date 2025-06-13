#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "NiagaraComponent.h"
#include "GameFramework/Actor.h"
#include "FieldActor.generated.h"

UENUM(BlueprintType)
enum class ESpawnSource : uint8
{
	Owner    UMETA(DisplayName="Owner"),
	Target   UMETA(DisplayName="Target")
};

UCLASS()
class CR4S_API AFieldActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AFieldActor();

	UFUNCTION(BlueprintCallable, Category="Boss|Skill")
	void Initialize(AActor* OwnerMonster, AActor* TargetActor = nullptr);

	FORCEINLINE void SetNiagaraActive(bool bActive) const { NiagaraComp->Activate(bActive); }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere, Category="Boss|Skill")
    class UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float InitCapsuleRadius = 100.f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float InitCapsuleHalfHeight = 100.f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	bool bFollowOwner = false;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	ESpawnSource SpawnSource = ESpawnSource::Owner;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float LifeTime = 5.f;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float DamageTickInterval = 0.5f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	FVector InitialNiagaraScale  = FVector(1.f);

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
    bool bDynamicSize = false;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill", meta=(EditCondition="bDynamicSize"))
	FVector DynamicScaleOffset = FVector(1.f);
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill", meta=(EditCondition="bDynamicSize", ClampMin="0.01"))
	float DynamicScaleDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, Category="Boss|Skill", meta=(EditCondition="bDynamicSize"))
	class UCurveVector* DynamicScaleCurve = nullptr;

private:
	void ApplyInitialScale();
	void PerformGroundTrace();
	void StartTimers();
	void UpdateDynamicScale(float DeltaTime);
	void FollowOwner();
	
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
    
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void EndSkill();
	void ApplyDamageTick();

	UPROPERTY()
	AActor* OwnerActor = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;
	
	UPROPERTY()
	TSet<AActor*> OverlappingActors;
	
	float ElapsedTime = 0.f;
	float TraceHeight = 1000.f;

	FTimerHandle LifeTimerHandle;
	FTimerHandle DamageTimerHandle;
	
	FString MyHeader = TEXT("FieldActor");
};
