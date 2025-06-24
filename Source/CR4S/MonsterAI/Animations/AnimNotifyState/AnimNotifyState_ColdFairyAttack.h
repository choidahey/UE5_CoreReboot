#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ColdFairyAttack.generated.h"

class UBlackboardComponent;
class AAIController;
class AColdFairyActor;

UCLASS()
class CR4S_API UAnimNotifyState_ColdFairyAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	TSoftClassPtr<AActor> SpawnActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	int32 NumSpawnActor = 10;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float Interval = 0.05f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnRadius = 500.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Attack")
	float SpawnHeightOffset = 300.f;

private:
	void SpawnOne(USkeletalMeshComponent* MeshComp, AAIController* AIC, UBlackboardComponent* BB);
	
	FTimerHandle SpawnTimerHandle;
	int32 CurrentSpawnIndex = 0;

	FString MyHeader = TEXT("NotifyState_ColdFairyAttack");
};
