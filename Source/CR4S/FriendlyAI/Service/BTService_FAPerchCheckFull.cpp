#include "BTService_FAPerchCheckFull.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "FriendlyAI/ETC/PerchedTarget.h"
#include "Algo/RandomShuffle.h"

UBTService_FAPerchCheckFull::UBTService_FAPerchCheckFull()
{
	NodeName = TEXT("Perch Check Full");
}

void UBTService_FAPerchCheckFull::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AFAAIController* AICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
	if (!AICon) return;

	APawn* ControlledPawn = AICon->GetPawn();
	if (!ControlledPawn) return;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	TArray<AActor*>& PerchTargets = AICon->FilteredPerchTargets;
   
	if (PerchTargets.Num() == 0) return;
   
	APerchedTarget* FirstPerch = Cast<APerchedTarget>(PerchTargets[0]);
	if (FirstPerch && FirstPerch->GetOccupied())
	{
		Algo::RandomShuffle(PerchTargets);
	}
   
	bool AllOccupied = true;
	for (AActor* Target : PerchTargets)
	{
		APerchedTarget* Perch = Cast<APerchedTarget>(Target);
		if (Perch && !Perch->GetOccupied())
		{
			AllOccupied = false;
			break;
		}
	}
   
	BlackboardComp->SetValueAsBool(TEXT("AllPerchesAreFull"), AllOccupied);
   
	if (AllOccupied && BlackboardComp->GetValueAsBool(TEXT("IsLookingForPerch")))
	{
		BlackboardComp->SetValueAsBool(TEXT("FlyAway"), true);
		BlackboardComp->SetValueAsBool(TEXT("IsLookingForPerch"), false);
		BlackboardComp->SetValueAsBool(TEXT("AllPerchesAreFull"), false);
	}
}