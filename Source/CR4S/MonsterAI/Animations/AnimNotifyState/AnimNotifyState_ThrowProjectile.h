#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ThrowProjectile.generated.h"

class ARotatingProjectile;
class ABaseMonster;

UCLASS()
class CR4S_API UAnimNotifyState_ThrowProjectile : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ARotatingProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LaunchSpeed = 1200.f;

private:
	TMap<TWeakObjectPtr<ABaseMonster>, TWeakObjectPtr<ARotatingProjectile>> SpawnedProjectiles;

};
