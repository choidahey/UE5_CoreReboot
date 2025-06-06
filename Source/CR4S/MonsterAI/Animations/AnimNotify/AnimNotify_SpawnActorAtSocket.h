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
	FName SocketName = TEXT("spine_01");

	UPROPERTY(EditAnywhere, Category = "Skill")
	FName SkillActorTag = TEXT("SkillActor");
};
