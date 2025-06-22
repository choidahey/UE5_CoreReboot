#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "GameFramework/Actor.h"
#include "IceSpike.generated.h"

// NOTICE :: I will delete this class
class UCapsuleComponent;

UCLASS()
class CR4S_API AIceSpike : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:	
	AIceSpike();

	UFUNCTION(BlueprintCallable, Category = "Boas|Skill")
	void Launch();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float LifeTime = 3.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float RiseDuration = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float RiseHeight = 30.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float PushHorizontalStrength = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boas|Skill")
	float PushUpStrength = 0.f;

private:
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	bool bRising = true;
	float ElapsedRise = 0.f;
	FVector InitialOffset;
	FVector TargetOffset;

	FString MyHeader = TEXT("IceSpike");
};