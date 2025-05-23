#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IceSpike.generated.h"

class UBoxComponent;

UCLASS()
class CR4S_API AIceSpike : public AActor
{
	GENERATED_BODY()
	
public:	
	AIceSpike();

	UFUNCTION(BlueprintCallable, Category = "Hiems|StrongAttack")
	static AIceSpike* SpawnIceSpikeAtPlayer(APawn* OwnerPawn, TSubclassOf<AIceSpike> SpikeClass);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hiems|StrongAttack")
	TSubclassOf<AIceSpike> IceSpikeClass;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hiems|StrongAttack")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hiems|StrongAttack")
	UBoxComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hiems|StrongAttack")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Hiems|StrongAttack")
	float LifeTime;

	UPROPERTY(EditAnywhere, Category = "Hiems|StrongAttack")
	float RiseDuration;

	UPROPERTY(EditAnywhere, Category = "Hiems|StrongAttack")
	float RiseHeight;

	UPROPERTY(EditAnywhere, Category = "Hiems|StrongAttack")
	float PushHorizontalStrength;

	UPROPERTY(EditAnywhere, Category = "Hiems|StrongAttack")
	float PushUpStrength;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool  bRising = true;
	float ElapsedRise = 0.f;
	FVector InitialOffset;
	FVector TargetOffset;

	FString MyHeader;
};
