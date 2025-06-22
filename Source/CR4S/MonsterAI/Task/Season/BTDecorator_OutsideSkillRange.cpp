#include "MonsterAI/Task/Season/BTDecorator_OutsideSkillRange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTDecorator_OutsideSkillRange::UBTDecorator_OutsideSkillRange()
{
	NodeName = TEXT("OutsideSkillRange");
	CurrentDistanceKey.SelectedKeyName = FSeasonBossAIKeys::CurrentDistance;
	CurrentSkillRangeKey.SelectedKeyName = FSeasonBossAIKeys::CurrentSkillRange;
}

bool UBTDecorator_OutsideSkillRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BB)) return false;
	
	float CurrentDistance = BB->GetValueAsFloat(CurrentDistanceKey.SelectedKeyName);
	float SkillRange = BB->GetValueAsFloat(CurrentDistanceKey.SelectedKeyName);
	
	return CurrentDistance > SkillRange;
}
