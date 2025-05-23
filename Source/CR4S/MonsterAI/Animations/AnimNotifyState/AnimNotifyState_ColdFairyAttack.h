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
	UAnimNotifyState_ColdFairyAttack();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	TSubclassOf<AActor> ColdFairyActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	int32 NumSpawnActor;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Interval;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnRadius;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnHeightOffset;


private:
	FTimerHandle LaunchTimerHandle;
	TArray<AColdFairyActor*> SpawnedFairies;

	FString MyHeader;
};
