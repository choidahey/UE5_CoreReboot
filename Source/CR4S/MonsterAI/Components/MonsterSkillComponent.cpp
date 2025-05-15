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

void UMonsterSkillComponent::InitializeMonsterSkills(const FName MonsterID)
{
	if (const UMonsterDataSubsystem* Subsys = GetWorld()->GetGameInstance()->GetSubsystem<UMonsterDataSubsystem>())
	{
		//const UDataTable* SkillTable = Subsys->GetMonsterSkillDataTable(MonsterID);
		//if (!SkillTable)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeMonsterSkills : Fail to load the monster skill data table."), *MyHeader);
		//	return;
		//}

		//SkillList.Empty();
		//bSkillReady.Empty();
		//SkillCooldownTimers.Empty();
		//
		//for (const auto& Row : SkillTable->GetRowMap())
		//{
		//	if (const FMonsterSkillRow* RowData = reinterpret_cast<FMonsterSkillRow*>(Row.Value))
		//	{
		//		SkillList.Add(*RowData);
		//		bSkillReady.Add(true);
		//		SkillCooldownTimers.Add(FTimerHandle());
		//	}
		//}

		UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeMonsterSkills : %d skills loaded from MonsterSkillTable."), *MyHeader, SkillList.Num());
	}
}

void UMonsterSkillComponent::PlayPreMontage(int32 Index)
{
	const FMonsterSkillRow& Skill = SkillList[Index];

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

	const FMonsterSkillRow& Skill = SkillList[Index];

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

const FMonsterSkillRow UMonsterSkillComponent::GetSkillData(int32 Index) const
{
	return SkillList.IsValidIndex(Index) ? SkillList[Index] : FMonsterSkillRow();
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
		bSkillReady[Index] = true;
	}
}
