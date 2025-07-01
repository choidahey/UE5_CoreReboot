#include "EQSTest_Pooled.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

UEQSTest_Pooled::UEQSTest_Pooled()
{
	ValidItemType = UEnvQueryItemType_Actor::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEQSTest_Pooled::RunTest(FEnvQueryInstance& QueryInstance) const
{
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		
		bool bIsValid = ItemActor && !ItemActor->IsActorBeingDestroyed() && ItemActor->GetActorEnableCollision();
		
		It.SetScore(TestPurpose, FilterType, bIsValid, true);
	}
}