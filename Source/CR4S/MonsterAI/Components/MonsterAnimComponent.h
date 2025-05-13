#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "MonsterAnimComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CR4S_API UMonsterAnimComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Constructor & Initialization

public:	
	UMonsterAnimComponent();

	/* Bind to SkeletalMesh & AnimInstance */
	void Initialize(USkeletalMeshComponent* InMesh);

#pragma endregion

#pragma region Montage References

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	UAnimMontage* StunnedMontage;

#pragma endregion

#pragma region Cached References

protected:
	TWeakObjectPtr<USkeletalMeshComponent> MeshComp;
	TWeakObjectPtr<UAnimInstance> AnimInstance;

#pragma endregion

#pragma region Montage Controls
public:
	void PlayMontange(UAnimMontage* Montage);
	bool IsPlayingMontage(UAnimMontage* Montage) const;
	void StopAllMontages();

	/* Explicit Play Calls */
	void PlayAttackMontage();
	void PlayDeathMontage();
	void PlayHitReactMontage();
	void PlayStunnedMontage();

#pragma endregion

private:
	FString MyHeader;

};
