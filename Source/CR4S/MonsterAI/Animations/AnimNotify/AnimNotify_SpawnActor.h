#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnActor.generated.h"

class ABlizzard;

UCLASS()
class CR4S_API UAnimNotify_SpawnActor : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	TSubclassOf<AActor> SpawnFieldActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	FVector SpawnLocationOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	float LifeTime = 0.f;

private:
	const float TraceHeight = 1000.f;
	FString MyHeader = TEXT("Notify_SpawnActor");
};
