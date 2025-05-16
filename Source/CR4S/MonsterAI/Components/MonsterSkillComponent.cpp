#include "MonsterSkillComponent.h"
#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

UMonsterSkillComponent::UMonsterSkillComponent()
	: MyHeader(TEXT("MonsterSkillComp"))
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMonsterSkillComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMonsterSkillComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] EndPlay : World is null, skipping timer cleanup."), *MyHeader);
		return;
	}

	FTimerManager& TimerManager = World->GetTimerManager();
	for (FTimerHandle& Handle : SkillCooldownTimers)
	{
		if (TimerManager.IsTimerActive(Handle))
		{
			TimerManager.ClearTimer(Handle);
		}
	}
}

void UMonsterSkillComponent::InitializeMonsterSkills(const FName MonsterID)
{
	if (const UMonsterDataSubsystem* Subsys = GetWorld()->GetGameInstance()->GetSubsystem<UMonsterDataSubsystem>())
	{
		SkillList.Empty();
		bSkillReady.Empty();
		SkillCooldownTimers.Empty();

		SkillList = Subsys->GetMonsterSkillData(MonsterID);

		if (SkillList.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeMonsterSkills : Fail to load the monster skill data table."), *MyHeader);
			return;
		}
		
		for (const auto& Row : SkillList)
		{
			bSkillReady.Add(true);
			SkillCooldownTimers.Add(FTimerHandle());
		}

		UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeMonsterSkills : %d skills loaded from MonsterSkillTable."), *MyHeader, SkillList.Num());
	}
}

void UMonsterSkillComponent::PlayPreMontage(int32 Index)
{
	const FMonsterSkillData& Skill = SkillList[Index];

	if (Skill.PreMontage.IsValid())
	{
		if (UAnimInstance* Anim = GetAnimInstance())
		{
			Anim->Montage_Play(Skill.PreMontage.Get());
		}
	}
}

void UMonsterSkillComponent::UseSkill(int32 Index)
{
	if (IsSkillReady(Index) == false)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] UseSkill : Skill %d is not ready."), *MyHeader, Index);
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[%s] UseSkill : Use skill %d."), *MyHeader, Index);

	const FMonsterSkillData& Skill = SkillList[Index];
	CurrentSkillIndex = Index;

	if (Skill.SkillMontage.IsValid())
	{
		if (UAnimInstance* Anim = GetAnimInstance())
		{
			Anim->Montage_Play(Skill.SkillMontage.Get());
		}
	}

	// Cooldown
	bSkillReady[Index] = false;

	GetWorld()->GetTimerManager().SetTimer(
		SkillCooldownTimers[Index],
		FTimerDelegate::CreateUObject(this, &UMonsterSkillComponent::ResetCooldown, Index),
		Skill.Cooldown,
		false
	);
}

bool UMonsterSkillComponent::IsSkillReady(int32 Index) const
{
	return bSkillReady.IsValidIndex(Index) && bSkillReady[Index];
}

float UMonsterSkillComponent::GetSkillRange(int32 Index) const
{
	return SkillList.IsValidIndex(Index) ? SkillList[Index].Range : 0.f;
}

const FMonsterSkillData& UMonsterSkillComponent::GetCurrentSkillData() const
{
	static const FMonsterSkillData Empty;
	return SkillList.IsValidIndex(CurrentSkillIndex) ? SkillList[CurrentSkillIndex] : Empty;
}

UAnimInstance* UMonsterSkillComponent::GetAnimInstance() const
{
	if (const ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Char->GetMesh())
			return Mesh->GetAnimInstance();
	}
	return nullptr;
}

void UMonsterSkillComponent::ResetCooldown(int32 Index)
{
	if (bSkillReady.IsValidIndex(Index))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] ResetCooldown : Use skill %d."), *MyHeader, Index);
		bSkillReady[Index] = true;
	}
}
