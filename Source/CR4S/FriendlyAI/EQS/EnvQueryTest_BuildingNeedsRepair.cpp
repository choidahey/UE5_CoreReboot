#include "EnvQueryTest_BuildingNeedsRepair.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Gimmick/Components/DestructibleComponent.h"

UEnvQueryTest_BuildingNeedsRepair::UEnvQueryTest_BuildingNeedsRepair()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_BuildingNeedsRepair::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		if (!ItemActor)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}
		
		UDestructibleComponent* DestructibleComp = ItemActor->FindComponentByClass<UDestructibleComponent>();
		if (!DestructibleComp)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		const float Current = DestructibleComp->GetCurrentHealth();
		const float Max = DestructibleComp->GetMaxHealth();
		
		if (Current >= Max)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}
		
		It.SetScore(TestPurpose, FilterType, true, true);
	}
}