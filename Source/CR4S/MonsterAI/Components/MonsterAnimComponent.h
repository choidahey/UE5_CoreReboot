#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/AnimMontage.h"
#include "MonsterAnimComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnimMontageEndedSignature, UAnimMontage*, Montage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CR4S_API UMonsterAnimComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Constructor & Initialization

public:	
	UMonsterAnimComponent();

	/* Bind to SkeletalMesh & AnimInstance */
	void Initialize(USkeletalMeshComponent* InMesh);

	FORCEINLINE TObjectPtr<UAnimMontage> GetDeathMontage() const { return DeathMontage; }

#pragma endregion

#pragma region Montage References

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	TObjectPtr<UAnimMontage> CombatMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	TObjectPtr<UAnimMontage> BerserkMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	TObjectPtr<UAnimMontage> StunnedMontage;

	// UPROPERTY(EditDefaultsOnly, Category = "Monster|Montage")
	// TObjectPtr<UAnimMontage> HitReactMontage;

#pragma endregion

#pragma region Cached References

protected:
	TWeakObjectPtr<UAnimInstance> AnimInstance;

#pragma endregion

#pragma region Montage Controls
public:
	void PlayMontage(UAnimMontage* Montage);
	bool IsAnyMontagePlaying() const;
	bool IsPlayingMontage(UAnimMontage* Montage) const;
	void StopAllMontages();


	/* Explicit Play Calls */
	void PlayCombatMontage();
	void PlayBerserkMontage();
	void PlayDeathMontage();
	void PlayStunnedMontage();
	// void PlayHitReactMontage();
	
	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnAnimMontageEndedSignature OnMontageEndedNotify;

protected:
	UFUNCTION()
	void Handle_OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

#pragma endregion

private:
	FString MyHeader;

};
