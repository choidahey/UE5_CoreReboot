#include "MonsterAI/Task/Season/BTService_CheckNearestHouse.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
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
	if (!BlackboardComp)
		return;
	
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!IsValid(PlayerChar))
		return;

	const FVector PlayerLoc = PlayerChar->GetActorLocation();
	AActor* NearestHouse = FindNearestHouse(PlayerLoc);

	BlackboardComp->SetValueAsObject(FSeasonBossAIKeys::NearestHouseActor, NearestHouse);
}

void UBTService_CheckNearestHouse::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackboardComp)) return;
	
	AActor* CurrentNearest = Cast<AActor>(BlackboardComp->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	
	if (!IsValid(CurrentNearest))
	{
		CR4S_Log(LogDa, Warning, TEXT("Invalid NearestHouse"));
		ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		if (!IsValid(PlayerChar)) return;

		const FVector PlayerLoc = PlayerChar->GetActorLocation();
		AActor* NewNearestHouse = FindNearestHouse(PlayerLoc);

		BlackboardComp->SetValueAsObject(FSeasonBossAIKeys::NearestHouseActor, NewNearestHouse);
	}
	else
	{
		CR4S_Log(LogDa, Warning, TEXT("Valid NearestHouse"));
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
