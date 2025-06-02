#include "MonsterAnimComponent.h"

UMonsterAnimComponent::UMonsterAnimComponent()
	: MyHeader(TEXT("MonsterAnimComponent"))
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterAnimComponent::Initialize(USkeletalMeshComponent* InMesh)
{
	if (!InMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Initialize : Invaild SkeletalMeshComponent"), *MyHeader);
		return;
	}

	AnimInstance = InMesh->GetAnimInstance();

	if (!AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Initialize : AnimInstance is null!"), *MyHeader);
		return;
	}
}

void UMonsterAnimComponent::PlayMontage(UAnimMontage* Montage)
{
	if (!Montage || !AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] PlayMontage: Invalid Montage or AnimInstance"), *MyHeader);
		return;
	}

	AnimInstance->Montage_Play(Montage, 1.f);
}

bool UMonsterAnimComponent::IsAnyMontagePlaying() const
{
	return AnimInstance.IsValid() && AnimInstance->IsAnyMontagePlaying();
}

bool UMonsterAnimComponent::IsPlayingMontage(UAnimMontage* Montage) const
{
	if (!Montage || !AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] IsPlayingMontage: Invalid Montage or AnimInstance"), *MyHeader);
		return false;
	}

	return AnimInstance->Montage_IsPlaying(Montage);
}

void UMonsterAnimComponent::StopAllMontages()
{
	if (!AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] StopAllMontages: Invalid AnimInstance"), *MyHeader);
		return;
	}

	AnimInstance->StopAllMontages(0.2f);
}

void UMonsterAnimComponent::PlayCombatMontage()
{
	PlayMontage(CombatMontage);
}

void UMonsterAnimComponent::PlayDeathMontage()
{
	PlayMontage(DeathMontage);
}

void UMonsterAnimComponent::PlayStunnedMontage()
{
	PlayMontage(StunnedMontage);
}
