#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "WeaponActor.generated.h"

UCLASS()
class CR4S_API AWeaponActor : public ABaseSkillActor
{
	GENERATED_BODY()

public:
	AWeaponActor();

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	void SetupIgnoreOwnerCollision();

public:
	void ActivateWeaponCollision();
	void DeactivateWeaponCollision();

private:
	void UpdateSkillDataFromOwner();

	UPROPERTY()
	bool bShouldActivateCollision = true;

};
