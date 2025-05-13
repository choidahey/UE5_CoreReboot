#include "MonsterAttributeComponent.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"


UMonsterAttributeComponent::UMonsterAttributeComponent()
	: MyHeader(TEXT("MonsterAttrComp"))
{
}

void UMonsterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	// NOTICE :: Test Call
	FName MonsterID = TEXT("MonsterTest1");
	InitializeMonsterAttribute(MonsterID);

	ApplyDamage(30.0f);
	UE_LOG(LogTemp, Log, TEXT("[%s] After Damage HP: %.2f / %.2f"), *MyHeader, CurrentHP, CurrentAttribute.MaxHP);
}

void UMonsterAttributeComponent::InitializeMonsterAttribute(const FName MonsterID)
{
	// TODO :: 홍일님 한거 참고해서 수정
	UGameInstance* GI = GetOwner()->GetWorld()->GetGameInstance();
	if (auto* Subsys = GI ? GI->GetSubsystem<UMonsterDataSubsystem>() : nullptr)
	{
		if (const FMonsterAttributeRow* Row = Subsys->GetMonsterAttributeRow(MonsterID))
		{
			CurrentAttribute = *Row;
			CurrentHP = CurrentAttribute.MaxHP;

			// NOTICE :: Test Log
			UE_LOG(LogTemp, Log, TEXT("[%s] %s Init: MaxHP=%.2f, AttackPower=%.2f, AttackRange=%.2f, AtkSpeed=%.2f, MoveSpeed=%.2f"),
				*MyHeader,
				*MonsterID.ToString(),
				CurrentAttribute.MaxHP,
				CurrentAttribute.AttackPower,
				CurrentAttribute.AttackRange,
				CurrentAttribute.AttackSpeed,
				CurrentAttribute.MoveSpeed
			);

			return;
		}
	}

	CurrentAttribute = FMonsterAttributeRow();
	CurrentHP = 0.0f;

	/*if (!IsValid(MonsterAttributeTable))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] InValid MonsterAttributeTable"), *MyHeader);
		return;
	}

	static const FString Context(TEXT("InitialAttrbute"));
	if (FMonsterAttributeRow* Row = MonsterAttributeTable->FindRow<FMonsterAttributeRow>(MonsterKey, Context))
	{
		CurrentAttribute = *Row;
		CurrentHP = Row->MaxHP;

		// NOTICE :: Test Log
		UE_LOG(LogTemp, Log, TEXT("[%s] %s Init: MaxHP=%.2f, AttackPower=%.2f, AttackRange=%.2f, AtkSpeed=%.2f, MoveSpeed=%.2f"),
			*MyHeader,
			*MonsterKey.ToString(),
			CurrentAttribute.MaxHP,
			CurrentAttribute.AttackPower,
			CurrentAttribute.AttackRange,
			CurrentAttribute.AttackSpeed,
			CurrentAttribute.MoveSpeed
		);
	}*/
}

void UMonsterAttributeComponent::ApplyDamage(float DamageAmount)
{
	ChangeHP(-FMath::Abs(DamageAmount));
}

void UMonsterAttributeComponent::ApplyHeal(float HealAmount)
{
	ChangeHP(FMath::Abs(HealAmount));
}

void UMonsterAttributeComponent::ChangeHP(float Delta)
{
	CurrentHP = FMath::Clamp(CurrentHP + Delta, 0.0f, CurrentAttribute.MaxHP);
	
	if (IsDead() && GetOwner())
	{
		GetOwner()->Destroy();
	}
}

bool UMonsterAttributeComponent::IsDead() const
{
	return CurrentHP <= 0.0f;
}


