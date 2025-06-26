#include "BTService_UpdateDistance.h"
#include "AIController.h"
#include "CR4S.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTService_UpdateDistance::UBTService_UpdateDistance()
{
	NodeName = TEXT("Update Distance");
	bNotifyTick = true;
	CurrentDistanceKey.SelectedKeyName = FSeasonBossAIKeys::CurrentDistance;
	CurrentSkillRangeKey.SelectedKeyName = FSeasonBossAIKeys::CurrentSkillRange;
	TargetActorKey.SelectedKeyName = FAIKeys::TargetActor;
}

void UBTService_UpdateDistance::TickNode(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;

	if (!BB || !Pawn)
	{
		if (bShowDebugInfo)
		{
			CR4S_Log(LogDa, Warning, TEXT("[%s] Missing Pawn/BB"), *NodeName);
		}
		return;
	}
	
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FAIKeys::TargetActor));

	if (!IsValid(Target))
	{
		Target = Cast<AActor>(BB->GetValueAsObject(FSeasonBossAIKeys::NearestHouseActor));
	}
	if (!IsValid(Target))
	{
		Target = UGameplayStatics::GetPlayerPawn(Pawn->GetWorld(), 0);
	}
	
	const float Dist = Pawn->GetDistanceTo(Target);
	BB->SetValueAsFloat(CurrentDistanceKey.SelectedKeyName, Dist);

	float CurrentDistance = BB->GetValueAsFloat(CurrentDistanceKey.SelectedKeyName);
	float SkillRange = BB->GetValueAsFloat(CurrentSkillRangeKey.SelectedKeyName);

	if (bShowDebugInfo)
	{
		CR4S_Log(LogDa, Log, TEXT("[%s] Distance: %.1f"), *NodeName, Dist);
	}
}