#include "MonsterAI/Task/Season/BTService_RandomDice.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_RandomDice::UBTService_RandomDice()
{
	NodeName = TEXT("RandomDice");
	bNotifyBecomeRelevant = true;
}

void UBTService_RandomDice::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (NumDice <= 0) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	
	const int32 RandIdx = FMath::RandRange(0, NumDice - 1);
	BB->SetValueAsInt(RandomIndexKey.SelectedKeyName, RandIdx);
}
