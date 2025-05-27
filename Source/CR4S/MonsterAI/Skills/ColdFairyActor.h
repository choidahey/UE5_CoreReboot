#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ColdFairyActor.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class CR4S_API AColdFairyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AColdFairyActor();

	void Launch();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	USphereComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Damage;

	FVector LaunchDirection;


private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& Sweep);

	FString MyHeader;
};
