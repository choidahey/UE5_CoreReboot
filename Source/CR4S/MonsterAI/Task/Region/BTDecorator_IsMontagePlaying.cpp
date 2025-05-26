#include "MonsterAI/Task/Region/BTDecorator_IsMontagePlaying.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

UBTDecorator_IsMontagePlaying::UBTDecorator_IsMontagePlaying()
{
	NodeName = TEXT("IsAnyMontagePlaying");
}

bool UBTDecorator_IsMontagePlaying::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster)
	{
		return false;
	}

	if (const UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>())
	{
		const bool bIsPlaying = AnimComp->IsAnyMontagePlaying();
		return bIsPlaying == bShouldBePlaying;
	}

	return false;
}
