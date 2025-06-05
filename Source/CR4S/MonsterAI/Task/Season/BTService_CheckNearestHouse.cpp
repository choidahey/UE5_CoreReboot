#include "MonsterAI/Task/Season/BTService_CheckNearestHouse.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTService_CheckNearestHouse::UBTService_CheckNearestHouse()
{
	NodeName = TEXT("CheckNearestHouse");
	Interval = 1.0f;
	bCreateNodeInstance = true;
}

void UBTService_CheckNearestHouse::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		FVector PlayerLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
		AActor* Nearest = FindNearestHouse(PlayerLoc);
		BlackboardComp->SetValueAsObject(NearestHouseActor.SelectedKeyName, Nearest);
	}
}

void UBTService_CheckNearestHouse::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;
	
	AActor* NearestHouse = Cast<AActor>(BlackboardComp->GetValueAsObject(NearestHouseActor.SelectedKeyName));
	
	if (!IsValid(NearestHouse))
	{
		const FVector PlayerLoc = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetActorLocation();
		
		AActor* NewNearestHouse = FindNearestHouse(PlayerLoc);
		
		BlackboardComp->SetValueAsObject(NearestHouseActor.SelectedKeyName, NewNearestHouse);
	}
}

AActor* UBTService_CheckNearestHouse::FindNearestHouse(const FVector& PlayerLocation) const
{
	TArray<AActor*> Houses;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), HouseTag, Houses);

	if (Houses.Num() <= 0)
	{
		return nullptr;
	}

	float NearestDistance = FLT_MAX;
	AActor* NearestActor = nullptr;

	for (AActor* FindActor : Houses)
	{
		if (!IsValid(FindActor)) continue;
		
		float Distance = FVector::DistSquared(FindActor->GetActorLocation(), PlayerLocation);

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestActor = FindActor;
		}
	}

	return NearestActor;
}
