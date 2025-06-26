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

	UPROPERTY(EditAnywhere, Category = "Skill")
	FVector SpawnOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Skill")
	FRotator AdditionalRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Skill")
	FVector SpawnScale = FVector(1.f, 1.f, 1.f);

	UPROPERTY(EditAnywhere, Category = "Skill|Projectile")
	float LaunchSpeed = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Skill|Projectile")
	float TravelDistance = 3000.f;

private:
	void SpawnSkillActor(USkeletalMeshComponent* MeshComp, AActor* Owner);

};
