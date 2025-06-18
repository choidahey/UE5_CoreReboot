#include "EnvQueryTest_BuildingNeedsRepair.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"

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
		
		ABaseBuildingGimmick* BuildingGimmick = Cast<ABaseBuildingGimmick>(ItemActor);
		if (!BuildingGimmick)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		const float DurabilityRatio = BuildingGimmick->GetDurabilityRatio();
		
		if (DurabilityRatio >= 1.0f)
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}
		
		It.SetScore(TestPurpose, FilterType, true, true);
	}
}