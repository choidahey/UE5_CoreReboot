#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingActor.generated.h"

class USphereComponent;
class AGeometryCollectionActor;

UCLASS()
class CR4S_API ARollingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ARollingActor();

	UFUNCTION(BlueprintCallable, Category = "Rolling")
	void LaunchInDirection(const FVector& Direction);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling|BreakEffect")
	TSubclassOf<AGeometryCollectionActor> BreakGeometryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	FVector ScaleFactor = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling") 
	float LaunchSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float Damage = 300.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float MaxDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float MaxLifetime = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Rolling|BreakEffect")
	float ImpulseStrength = 1500.f;

	UPROPERTY()
	bool bHasBroken = false;

private:
	UFUNCTION()
	void OnRollingOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* Other,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void BreakAndDestroy();

	FVector StartLocation;
	FVector LastKnownVelocity;
	FTimerHandle BreakTimerHandle;

};
