#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SpawnActorAtSocket.generated.h"

UCLASS()
class CR4S_API UAnimNotify_SpawnActorAtSocket : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Boss|Skill")
	TSubclassOf<AActor> SkillActorClass;

	UPROPERTY(EditAnywhere, Category = "Skill")
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Skill")
	FName SkillActorTag = TEXT("SkillActor");

	UPROPERTY(EditAnywhere, Category = "Skill")
	int32 SpawnCount = 1;

	UPROPERTY(EditAnywhere, Category = "Skill")
	bool bSpawnAllAtOnce = true;

	UPROPERTY(EditAnywhere, Category = "Skill", meta = (EditCondition = "!bSpawnAllAtOnce", ClampMin = "0.01"))
	float SpawnInterval = 0.2f;

private:
	void SpawnSkillActor(USkeletalMeshComponent* MeshComp, AActor* Owner);

};
