#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"
#include "CR4S/MonsterAI/Data/MonsterAttributeRow.h"
#include "CR4S/MonsterAI/Data/MonsterSkillData.h"

UMonsterDataSubsystem::UMonsterDataSubsystem()
	: MyHeader(TEXT("MonsterDataSubsyetem"))
{
    static ConstructorHelpers::FObjectFinder<UDataTable> MonsterAttributeDataTablePath(
        TEXT("DataTable'/Game/CR4S/_Blueprint/MonsterAI/Data/MonsterAttributeDataTable."
            "MonsterAttributeDataTable'"));

    if (MonsterAttributeDataTablePath.Succeeded())
    {
        MonsterAttributeTable = MonsterAttributeDataTablePath.Object;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to find MonsterAttributeDataTable Path"), *MyHeader);
    }

}

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOG(LogTemp, Log, TEXT("[%s] Loaded Attribute DataTable: %s"), *MyHeader, *MonsterAttributeTable->GetName());
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

    UDataTable* SkillTable = MonsterAttributeRow->SkillDataTable;
    if (!SkillTable)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[%s] SkillDataTable is null for %s"), *MyHeader, *MonsterID.ToString());
        return SkillDatas;
    }

    TArray<FMonsterSkillData*> SkillRows;
    SkillTable->GetAllRows<FMonsterSkillData>(TEXT("GetMonsterSkillData"), SkillRows);
    for (FMonsterSkillData* Row : SkillRows)
    {
        if (Row)
        {
            SkillDatas.Add(*Row);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[%s] Success Loaded %d skills for %s"), *MyHeader, SkillDatas.Num(), *MonsterID.ToString());

    return SkillDatas;
}
