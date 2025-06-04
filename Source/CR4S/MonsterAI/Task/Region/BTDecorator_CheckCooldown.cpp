#include "MonsterAI/Task/Region/BTDecorator_CheckCooldown.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"

UBTDecorator_CheckCooldown::UBTDecorator_CheckCooldown()
{
	NodeName = TEXT("CheckCooldown");
}

bool UBTDecorator_CheckCooldown::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || BlackboardKey.SelectedKeyName.IsNone())
	{
		return false;
	}

	const float LastTime = BB->GetValueAsFloat(BlackboardKey.SelectedKeyName);
	const float CurrentTime = OwnerComp.GetOwner()->GetWorld()->GetTimeSeconds();
	const float Elapsed = CurrentTime - LastTime;

	return Elapsed >= CooldownSeconds;
}
