#include "BTService_FAPerchCheck.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "GameFramework/Actor.h"
#include "AIController.h"

UBTService_FAPerchCheck::UBTService_FAPerchCheck()
{
	NodeName = TEXT("Check Perch Distance and Set FlyToPerch");
	bNotifyBecomeRelevant = true;
}

uint16 UBTService_FAPerchCheck::GetInstanceMemorySize() const
{
	return sizeof(FBTService_FAPerchCheckMemory);
}

void UBTService_FAPerchCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FBTService_FAPerchCheckMemory* Memory = reinterpret_cast<FBTService_FAPerchCheckMemory*>(NodeMemory);

	AAIController* AICon = Cast<AAIController>(OwnerComp.GetAIOwner());
	if (!AICon) return;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AFAAIController* FAAICon = Cast<AFAAIController>(AICon);
	if (!FAAICon || FAAICon->FilteredPerchTargets.Num() == 0) return;

	AActor* PerchTarget = FAAICon->FilteredPerchTargets[0];
	if (!PerchTarget) return;

	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PerchTarget->GetActorLocation());

	if (Distance < 600.f)
	{
		if (!Memory->bDidSetFlyToPerch)
		{
			BB->SetValueAsBool("FlyToPerch", true);
			Memory->bDidSetFlyToPerch = true;
		}
	}
	else
	{
		BB->SetValueAsBool("FlyToPerch", false);
		Memory->bDidSetFlyToPerch = false;
	}
}