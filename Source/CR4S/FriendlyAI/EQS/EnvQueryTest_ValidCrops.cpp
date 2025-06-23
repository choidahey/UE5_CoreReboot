#include "EnvQueryTest_ValidCrops.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"

UEnvQueryTest_ValidCrops::UEnvQueryTest_ValidCrops()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_ValidCrops::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		ACropsGimmick* Crop = Cast<ACropsGimmick>(ItemActor);

		if (!Crop || !Crop->GetIsHarvestable())
		{
			It.ForceItemState(EEnvItemStatus::Failed);
			continue;
		}

		It.SetScore(TestPurpose, FilterType, true, true);
	}
}