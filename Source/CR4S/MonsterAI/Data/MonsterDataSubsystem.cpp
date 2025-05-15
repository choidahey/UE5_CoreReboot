#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FSoftObjectPath Path(TEXT("/Game/CR4S/_MonsterAI/Data/MonsterAttributeDataTable.MonsterAttributeDataTable"));
	MonsterAttributeTable = Cast<UDataTable>(Path.TryLoad());

	CacheMonsterSkillData();
}

FMonsterAttributeRow* UMonsterDataSubsystem::GetMonsterAttributeData(FName MonsterID) const
{
	if (MonsterAttributeTable && MonsterID.IsValid())
	{
		return MonsterAttributeTable->FindRow<FMonsterAttributeRow>(MonsterID, TEXT("Fail GetMonsterAttributeData"));
	}

	return nullptr;
}

const TArray<FMonsterSkillData>& UMonsterDataSubsystem::GetMonsterSkillData(FName MonsterID) const
{
    const TArray<FMonsterSkillData>* SkillDatas = CacheSkillData.Find(MonsterID);
	if (SkillDatas != nullptr)
	{
		// NOTICE :: Test Log
		UE_LOG(LogTemp, Log,
			TEXT("[MonsterDataSubsystem] %s has %d cached skills"),
			*MonsterID.ToString(),
			SkillDatas->Num()
		);

		for (int32 i = 0; i < SkillDatas->Num(); ++i)
		{
			const FMonsterSkillData& Skill = (*SkillDatas)[i];
			UE_LOG(LogTemp, Log,
				TEXT("%s : Damage = %.1f  Cooldown = %.1f  Range = %.1f"),
				*Skill.SkillName.ToString(),
				Skill.Damage,
				Skill.Cooldown,
				Skill.Range
			);
		}

		return *SkillDatas;
	}

	static const TArray<FMonsterSkillData> EmptyArray;

    return EmptyArray;
}

void UMonsterDataSubsystem::CacheMonsterSkillData()
{
	const TMap<FName, uint8*>& RowMap = MonsterAttributeTable->GetRowMap();
	for (TMap<FName, uint8*>::TConstIterator It(RowMap); It; ++It)
	{
		const FMonsterAttributeRow* AttrRow = reinterpret_cast<const FMonsterAttributeRow*>(It.Value());
		if (AttrRow == nullptr || !AttrRow->SkillDataTable.IsValid()) continue;

		UDataTable* SkillTable = AttrRow->SkillDataTable.LoadSynchronous();
		if (SkillTable == nullptr) continue;

		TArray<FMonsterSkillData*> SkillRows;
		SkillTable->GetAllRows<FMonsterSkillData>(TEXT("InitializeMonserSkill"), SkillRows);

		TArray<FMonsterSkillData>& CacheData = CacheSkillData.FindOrAdd(AttrRow->MonsterID);

		for (int32 i = 0; i < SkillRows.Num(); ++i)
		{
			FMonsterSkillData* RowData = SkillRows[i];
			if (RowData == nullptr)
			{
				continue;
			}

			FMonsterSkillData Copy = *RowData;

			if (Copy.PreMontage.IsValid())
			{
				Copy.PreMontage.LoadSynchronous();
			}
			if (Copy.SkillMontage.IsValid())
			{
				Copy.SkillMontage.LoadSynchronous();
			}

			CacheData.Add(Copy);
		}
	}
}

