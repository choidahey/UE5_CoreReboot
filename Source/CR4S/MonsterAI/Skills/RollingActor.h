#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollingActor.generated.h"

class UCapsuleComponent;
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

	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	TObjectPtr<UCapsuleComponent> CollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Rolling")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, Category = "BreakEffect")
	TSubclassOf<AGeometryCollectionActor> BreakGeometryClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rolling")
	float LaunchSpeed = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float Damage = 300.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float MaxDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Rolling")
	float MaxLifetime = 10.0f;

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
	FTimerHandle BreakTimerHandle;

};
