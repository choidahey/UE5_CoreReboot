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

	MeshComp = InMesh;
	AnimInstance = InMesh->GetAnimInstance();

	if (!AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Initialize : AnimInstance is null!"), *MyHeader);
		return;
	}
}

void UMonsterAnimComponent::PlayMontange(UAnimMontage* Montage)
{
	if (!Montage || !AnimInstance.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] PlayMontage: Invalid Montage or AnimInstance"), *MyHeader);
		return;
	}

	AnimInstance->Montage_Play(Montage, 1.f);
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

void UMonsterAnimComponent::PlayAttackMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] PlayAttackMontage"), *MyHeader);
	PlayMontange(AttackMontage);
}

void UMonsterAnimComponent::PlayDeathMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] PlayDeathMontage"), *MyHeader);
	PlayMontange(DeathMontage);
}

void UMonsterAnimComponent::PlayHitReactMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] PlayHitReactMontage"), *MyHeader);
	PlayMontange(HitReactMontage);
}

void UMonsterAnimComponent::PlayStunnedMontage()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] PlayStunnedMontage"), *MyHeader);
	PlayMontange(StunnedMontage);
}
