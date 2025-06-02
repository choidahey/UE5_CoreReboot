#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AcidWaveActor.generated.h"

class UBoxComponent;

UCLASS()
class CR4S_API AAcidWaveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAcidWaveActor();

	void Initialize(AActor* InOwner, AActor* InTarget);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	UBoxComponent* BoxCollisionComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float Damage{0.f};

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	FVector PositionOffset = FVector::ZeroVector;
	
	/* Rotation */
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	bool bIsRotating{false};

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float RotationDuration{1.f};

	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	float RotationStartDelay{2.f};

private:
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void ApplyPeriodicDamage(AActor* AffectedActor);
	void CleanupSelf();

	UPROPERTY()
	AActor* OwnerActor{nullptr};

	UPROPERTY()
	AActor* TargetActor{nullptr};
	
	UPROPERTY()
	TMap<AActor*, FTimerHandle> DebuffTimerMap;
	
	UPROPERTY()
	TMap<AActor*, int32> DebuffTicksMap;
	
	FRotator InitialRotation;
	
	float ElapsedRotationTime{0.f};
	float ElapsedDelayTime{0.f};

	FString MyHeader{TEXT("AcidWave")};
};
