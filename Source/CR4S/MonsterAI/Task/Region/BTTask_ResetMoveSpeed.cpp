#include "MonsterAI/Task/Region/BTTask_ResetMoveSpeed.h"
#include "AIController.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_ResetMoveSpeed::UBTTask_ResetMoveSpeed()
{
	NodeName = TEXT("Reset Move Speed");
}

EBTNodeResult::Type UBTTask_ResetMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!IsValid(Monster)) return EBTNodeResult::Failed;

	UCharacterMovementComponent* MoveComp = Monster->FindComponentByClass<UCharacterMovementComponent>();
	UMonsterAttributeComponent* AttributeComp = Monster->FindComponentByClass<UMonsterAttributeComponent>();

	if (Monster && IsValid(MoveComp) && IsValid(AttributeComp))
	{
		float DefaultSpeed = AttributeComp->GetMonsterAttribute().MoveSpeed;
		MoveComp->MaxWalkSpeed = DefaultSpeed;
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
