#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterSkillData.h"

UMonsterDataSubsystem::UMonsterDataSubsystem()
	: MonsterAttributeTablePtr(FSoftObjectPath(TEXT("/Game/CR4S/_Blueprint/MonsterAI/Data/MonsterAttributeDataTable.MonsterAttributeDataTable")))
    , MyHeader(TEXT("MonsterDataSubsyetem"))
{
}

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MonsterAttributeTable = MonsterAttributeTablePtr.LoadSynchronous();
}

FMonsterAttributeRow* UMonsterDataSubsystem::GetMonsterAttributeData(const FName MonsterID) const
{
	if (!MonsterAttributeTable || !MonsterID.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Invalid MonsterAttributeTable or MonsterID"), *MyHeader);
		return nullptr;
	}

	return MonsterAttributeTable->FindRow<FMonsterAttributeRow>(MonsterID, TEXT("Fail GetMonsterAttributeData"));
}

TArray<FMonsterSkillData> UMonsterDataSubsystem::GetMonsterSkillData(const FName MonsterID) const
{
    TArray<FMonsterSkillData> SkillDatas;

    FMonsterAttributeRow* MonsterAttributeRow = GetMonsterAttributeData(MonsterID);
    if (!MonsterAttributeRow)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[%s] No AttributeRow for %s"), *MyHeader, *MonsterID.ToString());
        return SkillDatas;
    }

    FSoftObjectPath SkillPath = MonsterAttributeRow->SkillDataTable.ToSoftObjectPath();
    if (!SkillPath.IsValid())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[%s] SkillDataTable path invalid for %s"), *MyHeader, *MonsterID.ToString());
        return SkillDatas;
    }

    UDataTable* SkillTable = Cast<UDataTable>(SkillPath.TryLoad());
    if (!SkillTable)
    {
        UE_LOG(LogTemp, Error,
            TEXT("[%s] Failed to load SkillDataTable at %s"), *MyHeader, *SkillPath.ToString());
        return SkillDatas;
    }

    TArray<FMonsterSkillData*> SkillRows;
    SkillTable->GetAllRows<FMonsterSkillData>(TEXT("GetMonsterSkillData"), SkillRows);
    for (FMonsterSkillData* Row : SkillRows)
    {
        if (!Row) continue;
        FMonsterSkillData Copy = *Row;
        if (Copy.PreMontage.IsValid())  Copy.PreMontage.LoadSynchronous();
        if (Copy.SkillMontage.IsValid()) Copy.SkillMontage.LoadSynchronous();
        SkillDatas.Add(MoveTemp(Copy));
    }

    UE_LOG(LogTemp, Log, TEXT("[%s] Success Loaded %d skills for %s"), *MyHeader, SkillDatas.Num(), *MonsterID.ToString());

    return SkillDatas;
}
