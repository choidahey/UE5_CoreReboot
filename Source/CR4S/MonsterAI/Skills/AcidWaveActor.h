#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "GameFramework/Actor.h"
#include "AcidWaveActor.generated.h"

class UBoxComponent;

UCLASS()
class CR4S_API AAcidWaveActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:	
	AAcidWaveActor();

	void Initialize(AActor* InOwner, AActor* InTarget);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bDrawLine = false;
	
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bIsRotating = false;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill", meta=(EditCondition="bIsRotating"))
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill", meta=(EditCondition="bIsRotating"))
	float RotationDuration = 1.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill", meta=(EditCondition="bIsRotating"))
	float RotationStartDelay = 2.f;

#pragma region Dymanic Scale
protected:
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bDynamicScale{false};

	UPROPERTY(EditAnywhere, Category = "Boss|Skill", meta=(EditCondition="bDynamicScale"))
	FVector ScaleOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill", meta=(EditCondition="bDynamicScale"))
	float ScaleDuration = 1.f;

#pragma endregion Dymanic Scale

private:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	void ApplyPeriodicDamage(AActor* AffectedActor);
	void CleanupSelf();
	void AlignPivotToBottom() const;

	UPROPERTY()
	AActor* OwnerActor{nullptr};

	UPROPERTY()
	AActor* TargetActor{nullptr};
	
	UPROPERTY()
	TMap<AActor*, FTimerHandle> DebuffTimerMap;
	
	UPROPERTY()
	TMap<AActor*, int32> DebuffTicksMap;
	
	FRotator InitialRotation;
	FVector InitialScale;
	FVector TargetScale;
	FVector InitialBoxExtent;
	
	float ElapsedRotationTime = 0.f;
	float ElapsedDelayTime = 0.f;
	float ElapsedScaleTime = 0.f;

	FString MyHeader{TEXT("AcidWave")};
};
