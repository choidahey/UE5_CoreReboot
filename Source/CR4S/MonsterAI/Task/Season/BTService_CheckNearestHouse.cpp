#include "MonsterAI/Task/Season/BTService_CheckNearestHouse.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_CheckNearestHouse::UBTService_CheckNearestHouse()
{
	NodeName = TEXT("CheckNearestHouse");
	Interval = 0.5f;
	bNotifyBecomeRelevant = true; 
	bNotifyTick = true;  
	bCreateNodeInstance = true;
}

void UBTService_CheckNearestHouse::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!IsValid(AIC)) return;
	
	APawn* OwnerPawn = AIC->GetPawn();
	if (!IsValid(OwnerPawn)) return;

	CachedOwnerPawn = OwnerPawn;
	const FVector OwnerPawnLoc = CachedOwnerPawn->GetActorLocation();
	AActor* NearestHouse = FindNearestHouse(OwnerPawnLoc);
	float HouseAttackRange = BlackboardComp->GetValueAsFloat(HouseAcceptanceRadiusKey.SelectedKeyName);

	BlackboardComp->SetValueAsObject(NearestHouseActor.SelectedKeyName, NearestHouse);
	BlackboardComp->SetValueAsFloat(HouseAcceptanceRadiusKey.SelectedKeyName,
		HouseAttackRange + CalculateActorRadius(NearestHouse)
		);
}

void UBTService_CheckNearestHouse::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;
	
	AActor* CurrentNearest = Cast<AActor>(BlackboardComp->GetValueAsObject(NearestHouseActor.SelectedKeyName));
	
	if (!IsValid(CurrentNearest))
	{
		CR4S_Log(LogDa, Warning, TEXT("[CheckNearestHouse] Invalid NearestHouse"));
		if (!IsValid(CachedOwnerPawn)) return;

		const FVector OwnerPawnLoc = CachedOwnerPawn->GetActorLocation();
		AActor* NewNearestHouse = FindNearestHouse(OwnerPawnLoc);

		BlackboardComp->SetValueAsObject(FSeasonBossAIKeys::NearestHouseActor, NewNearestHouse);
	}
}

AActor* UBTService_CheckNearestHouse::FindNearestHouse(const FVector& PlayerLocation) const
{
	TArray<AActor*> Houses;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), HouseTag, Houses);

	if (Houses.Num() <= 0)
		return nullptr;

	float NearestDistance = TNumericLimits<float>::Max();
	AActor* NearestActor = nullptr;

	for (AActor* HouseActor  : Houses)
	{
		if (!IsValid(HouseActor )) continue;
		
		float Distance = FVector::DistSquared(HouseActor ->GetActorLocation(), PlayerLocation);

		if (Distance < NearestDistance)
		{
			NearestDistance = Distance;
			NearestActor = HouseActor ;
		}
	}

	return NearestActor;
}

float UBTService_CheckNearestHouse::CalculateActorRadius(const AActor* InActor) const
{
	if (!IsValid(InActor))
		return 0.f;
	
	FVector Center, BoxExtent;
	InActor->GetActorBounds(false, Center, BoxExtent);
	const float BaseRadius = BoxExtent.GetMax();

	CR4S_Log(LogDa, Log, TEXT("house Radius : %f"), BaseRadius + RadiusOffset);

	return BaseRadius + RadiusOffset;
}
