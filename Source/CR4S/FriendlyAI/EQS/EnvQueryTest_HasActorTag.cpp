#include "EnvQueryTest_HasActorTag.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Actor.h"

UEnvQueryTest_HasActorTag::UEnvQueryTest_HasActorTag()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_HasActorTag::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		if (!ItemActor)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		const bool bHasTag = ItemActor->Tags.Contains(FilterTag);
		if (!bHasTag)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		It.SetScore(TestPurpose, FilterType, true, true);
	}
}
