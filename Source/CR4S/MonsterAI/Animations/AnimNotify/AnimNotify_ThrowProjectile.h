#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ThrowProjectile.generated.h"

UCLASS()
class CR4S_API UAnimNotify_ThrowProjectile : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, Category = "ThrowProjectile")
	TSubclassOf<class ARotatingProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "ThrowProjectile")
	FName AttachSocketName = TEXT("AxeWeapon_R");

	UPROPERTY(EditAnywhere, Category = "ThrowProjectile")
	float LaunchSpeed = 1000.f;

};
