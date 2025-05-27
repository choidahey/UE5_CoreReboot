#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IceSpike.generated.h"

class UCapsuleComponent;

UCLASS()
class CR4S_API AIceSpike : public AActor
{
	GENERATED_BODY()
	
public:	
	AIceSpike();

	UFUNCTION(BlueprintCallable, Category = "Boas|Skill")
	void Launch();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boas|Skill")
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boas|Skill")
	UCapsuleComponent* CollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boas|Skill")
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boas|Skill")
	float BoxSize  = 50.f;
	
	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float LifeTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float RiseDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float RiseHeight = 30.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float PushHorizontalStrength = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float PushUpStrength = 0.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float Damage = 0.f;

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

	bool  bRising = true;
	float ElapsedRise = 0.f;
	FVector InitialOffset;
	FVector TargetOffset;

	FString MyHeader = TEXT("IceSpike");
};