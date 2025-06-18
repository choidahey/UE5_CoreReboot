#include "EnvQueryTest_BuildingHealthScore.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"

UEnvQueryTest_BuildingHealthScore::UEnvQueryTest_BuildingHealthScore()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(true);
}

void UEnvQueryTest_BuildingHealthScore::RunTest(FEnvQueryInstance& QueryInstance) const
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
		const float Score = FMath::Pow(1.0f - DurabilityRatio, 2.0f);
		It.SetScore(TestPurpose, FilterType, Score, 0.0f, 1.0f);
	}
}