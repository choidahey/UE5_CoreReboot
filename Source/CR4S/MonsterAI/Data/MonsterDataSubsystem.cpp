#include "CR4S/MonsterAI/Data/MonsterDataSubsystem.h"

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const FSoftObjectPath Path(TEXT("/Game/CR4S/_MonsterAI/Data/Unreal_MonsterAttributeComponent.Unreal_MonsterAttributeComponent"));
	MonsterAttributeTable = Cast<UDataTable>(Path.TryLoad());
}

const FMonsterAttributeRow* UMonsterDataSubsystem::GetMonsterAttributeRow(FName MonsterID) const
{
	if (MonsterAttributeTable && MonsterID.IsValid())
	{
		return MonsterAttributeTable->FindRow<FMonsterAttributeRow>(MonsterID, TEXT("Fail GetMonsterAttributeRow"));
	}

	return nullptr;
}
