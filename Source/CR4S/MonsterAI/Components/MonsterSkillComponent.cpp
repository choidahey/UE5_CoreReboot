#include "MonsterSkillComponent.h"
#include "MonsterAI/Data/MonsterDataSubsystem.h"
#include "MonsterAI/BaseMonster.h"
#include "Components/CapsuleComponent.h"
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

	if (const ABaseMonster* Monster = Cast<ABaseMonster>(GetOwner()))
	{
		InitializeMonsterSkills(Monster->MonsterID);
	}

	if (const AActor* Owner = GetOwner())
	{
		TArray<UCapsuleComponent*> CapsuleComps;
		GetOwner()->GetComponents<UCapsuleComponent>(CapsuleComps);

		for (UCapsuleComponent* Comp : CapsuleComps)
		{
			if (Comp->ComponentHasTag("WeaponCollider"))
			{
				WeaponColliders.Add(Comp);
				Comp->OnComponentBeginOverlap.AddDynamic(this, &UMonsterSkillComponent::OnAttackHit);
				UE_LOG(LogTemp, Warning, TEXT("[SkillComp] BeginPlay : Bind Overlap function with WeaponCollider."));
			}
			else if (Comp->ComponentHasTag("BodyCollider"))
			{
				BodyColliders.Add(Comp);
				Comp->OnComponentBeginOverlap.AddDynamic(this, &UMonsterSkillComponent::OnAttackHit);
				UE_LOG(LogTemp, Warning, TEXT("[SkillComp] BeginPlay : Bind Overlap function with BodyCollider."));
			}
		}
	}
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

	if (IsValid(Skill.SkillMontage))
	{
		if (UAnimInstance* Anim = GetAnimInstance())
		{
			Anim->Montage_Play(Skill.SkillMontage);
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

TArray<int32> UMonsterSkillComponent::GetAvailableSkillIndices() const
{
	TArray<int32> SkillIndices;
	for (int32 i = 0; i < SkillList.Num(); ++i)
	{
		UE_LOG(LogTemp, Log, TEXT("%s : bSkillReday = %d"), *SkillList[i].SkillName.ToString(), (bSkillReady[i] ? 1 : 0));

		if (bSkillReady.IsValidIndex(i) && bSkillReady[i])
		{
			SkillIndices.Add(i);
		}
	}
	return SkillIndices;
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
		UE_LOG(LogTemp, Log, TEXT("[%s] ResetCooldown : Reset skill cooldown : %d."), *MyHeader, Index);
		bSkillReady[Index] = true;
	}
}

void UMonsterSkillComponent::SetAttackCollisionEnabled(bool bEnable, int32 InSkillIndex)
{
	CurrentSkillIndex = InSkillIndex;
	const FMonsterSkillData& Skill = SkillList[CurrentSkillIndex];

	if (Skill.bUseWeaponCollision && !WeaponColliders.IsEmpty())
	{
		for (UCapsuleComponent* Collider : WeaponColliders)
		{
			Collider->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		}
	}

	if (Skill.bUseBodyCollision && !BodyColliders.IsEmpty())
	{
		for (UCapsuleComponent* Collider : WeaponColliders)
		{
			Collider->SetCollisionEnabled(bEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		}
	}

	if (bEnable)
	{
		AlreadyHitActors.Empty();
	}
}

void UMonsterSkillComponent::OnAttackHit(
	UPrimitiveComponent* HitComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bIsAttackActive || !OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	const FMonsterSkillData& CurrentSkillData = SkillList[CurrentSkillIndex];

	if (!CurrentSkillData.bAllowMultipleHits && AlreadyHitActors.Contains(OtherActor))
	{
		return;
	}

	float Damage = CurrentSkillData.Damage;
	if (WeaponColliders.Contains(HitComp) && CurrentSkillData.bUseWeaponCollision)
	{
		Damage = CurrentSkillData.Damage;
	}
	else if (BodyColliders.Contains(HitComp) && CurrentSkillData.bUseBodyCollision)
	{
		Damage *= CurrentSkillData.BodyDamage;
	}
	else
	{
		return;
	}

	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	AlreadyHitActors.Add(OtherActor);
}
