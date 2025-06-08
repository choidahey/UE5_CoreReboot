#include "EnvQueryTest_ValidTree.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"

UEnvQueryTest_ValidTree::UEnvQueryTest_ValidTree()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_ValidTree::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		ATreeGimmick* Tree = Cast<ATreeGimmick>(ItemActor);

		if (!Tree || Tree->IsTrunkDestroyed())
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		It.SetScore(TestPurpose, FilterType, true, true);
	}
}
