#include "BTTask_HelperChopWood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../Gimmick/Components/DestructibleComponent.h"
#include "../../Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"
#include "../BaseHelperBot.h"
#include "../../Inventory/Components/PlayerInventoryComponent.h"
#include "Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"
#include "../Controller/HelperBotAIController.h"

UBTTask_HelperChopWood::UBTTask_HelperChopWood()
{
	NodeName = TEXT("HelperChopWood");
	bNotifyTick = true;	
}

uint16 UBTTask_HelperChopWood::GetInstanceMemorySize() const
{
	return sizeof(FBTChopWoodMemory);
}

EBTNodeResult::Type UBTTask_HelperChopWood::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTChopWoodMemory* MyMemory = reinterpret_cast<FBTChopWoodMemory*>(NodeMemory);
	if (!MyMemory) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;
	
	ATreeGimmick* Tree = Cast<ATreeGimmick>(TargetActor);
	if (Tree && Tree->IsTrunkDestroyed()) return EBTNodeResult::Failed;

	MyMemory->CachedTarget = TargetActor;
	MyMemory->CachedHelper = Pawn;

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(Pawn);
	if (!Helper) return EBTNodeResult::Failed;

	MyMemory->CachedDamagePerSecond = Helper->GetWoodDamagePerSecond();
	Helper->SetIsWorking(true);
	Helper->UpdateEyeBeamWorkTarget(TargetActor);
	
	GetWorld()->GetTimerManager().SetTimer(
		MyMemory->BeamUpdateTimer,
		FTimerDelegate::CreateLambda([Helper, TargetActor]()
		{
			if (Helper && TargetActor)
			{
				Helper->UpdateEyeBeamWorkTarget(TargetActor);
			}
		}),
		1.0f,
		true
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_HelperChopWood::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTChopWoodMemory* MyMemory = reinterpret_cast<FBTChopWoodMemory*>(NodeMemory);
	if (!MyMemory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	if (!MyMemory->CachedTarget.IsValid() || !MyMemory->CachedHelper.IsValid())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(MyMemory->CachedHelper.Get());
	if (!Helper)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	AActor* Target = MyMemory->CachedTarget.Get();
	if (!Target)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	const float DamageThisFrame = MyMemory->CachedDamagePerSecond * DeltaSeconds;
		
	ATreeGimmick* Tree = Cast<ATreeGimmick>(Target);
	if (Tree && Tree->IsTrunkDestroyed())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	if (Target->IsActorBeingDestroyed())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}

	UGameplayStatics::ApplyDamage(Target, DamageThisFrame, Helper->GetController(), Helper, UDamageType::StaticClass());
}

void UBTTask_HelperChopWood::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result)
{
	FBTChopWoodMemory* MyMemory = reinterpret_cast<FBTChopWoodMemory*>(NodeMemory);
	if (!MyMemory)
	{
		FinishLatentTask(OwnerComp, Result);
		return;
	}

	if (MyMemory->BeamUpdateTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MyMemory->BeamUpdateTimer);
	}

	if (MyMemory->CachedHelper.IsValid())
	{
		ABaseHelperBot* Helper = Cast<ABaseHelperBot>(MyMemory->CachedHelper.Get());
		if (Helper)
		{
			Helper->SetIsWorking(false);
			Helper->StopEyeBeamWork();
		}
	}
    
	FinishLatentTask(OwnerComp, Result);
}

EBTNodeResult::Type UBTTask_HelperChopWood::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Aborted);
	return EBTNodeResult::Aborted;
}