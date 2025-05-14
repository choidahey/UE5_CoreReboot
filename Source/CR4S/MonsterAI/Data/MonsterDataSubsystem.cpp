#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FSoftObjectPath Path(TEXT("/Game/CR4S/_MonsterAI/Data/MonsterAttributeDataTable.MonsterAttributeDataTable"));
	MonsterAttributeTable = Cast<UDataTable>(Path.TryLoad());

	const FSoftObjectPath Path(TEXT("/Game/CR4S/_MonsterAI/Data/MonsterSkillDataTable.MonsterSkillDataTable"));
	MonsterSkillTable = Cast<UDataTable>(Path.TryLoad());
}

const FMonsterAttributeRow* UMonsterDataSubsystem::GetMonsterAttributeData(FName MonsterID) const
{
	if (MonsterAttributeTable && MonsterID.IsValid())
	{
		return MonsterAttributeTable->FindRow<FMonsterAttributeRow>(MonsterID, TEXT("Fail GetMonsterAttributeData"));
	}

	return nullptr;
}

const FMonsterSkillRow* UMonsterDataSubsystem::GetMonsterSkillData(FName SkillID) const
{
	if (MonsterSkillTable && SkillID.IsValid())
	{
		return MonsterSkillTable->FindRow<FMonsterSkillRow>(SkillID, TEXT("Fail GetSkillData"));
	}

	return nullptr;
}