#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ColdFairyAttack.generated.h"

class AColdFairyActor;

UCLASS()
class CR4S_API UAnimNotifyState_ColdFairyAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	TSubclassOf<AActor> ColdFairyActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	int32 NumSpawnActor = 10;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Interval = 1.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnHeightOffset = 300.f;


private:
	FTimerHandle LaunchTimerHandle;

	UPROPERTY()
	TArray<AColdFairyActor*> SpawnedFairies;

	FString MyHeader = TEXT("NotifyState_ColdFairyAttack");
};
