#include "BTTask_HelperRepair.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Gimmick/GimmickObjects/Buildings/BaseBuildingGimmick.h"
#include "FriendlyAI/BaseHelperBot.h"
#include "FriendlyAI/Controller/HelperBotAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/Engine.h"

UBTTask_HelperRepair::UBTTask_HelperRepair()
{
    NodeName = TEXT("HelperRepair");
    bNotifyTick = true;
}

uint16 UBTTask_HelperRepair::GetInstanceMemorySize() const
{
	return sizeof(FBTRepairMemory);
}

void UBTTask_HelperRepair::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitMemoryType) const
{
	if (InitMemoryType == EBTMemoryInit::Initialize)
	{
		new(NodeMemory) FBTRepairMemory();
	}
}

EBTNodeResult::Type UBTTask_HelperRepair::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTRepairMemory* MyMemory = reinterpret_cast<FBTRepairMemory*>(NodeMemory);
	if (!MyMemory) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	if (!TargetActor->IsA(ABaseBuildingGimmick::StaticClass()))
	{
		return EBTNodeResult::Failed;
	}
	
	MyMemory->CachedTarget = TargetActor;
	MyMemory->CachedHelper = Pawn;
	
	MyMemory->OwnerComp = &OwnerComp;
	MyMemory->NodeMemoryPtr = NodeMemory;
	
	if (TargetActor && !MyMemory->bIsTargetDestroyedBound)
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
		TargetActor->OnDestroyed.AddDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
		MyMemory->bIsTargetDestroyedBound = true;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(Pawn);
	if (!Helper) return EBTNodeResult::Failed;

	MyMemory->CachedRepairPerSecond = Helper->GetRepairingPerSecond();
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

void UBTTask_HelperRepair::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTRepairMemory* MyMemory = reinterpret_cast<FBTRepairMemory*>(NodeMemory);
	if (!MyMemory)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
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
	if (!Target || Target->IsActorBeingDestroyed())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}

	ABaseBuildingGimmick* BuildingGimmick = Cast<ABaseBuildingGimmick>(Target);
	if (!BuildingGimmick)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}
	
	if (BuildingGimmick->GetDurabilityRatio() >= 1.0f)
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}
	
	const float RepairAmount = MyMemory->CachedRepairPerSecond * DeltaSeconds;
	BuildingGimmick->OnRepairBuilding(RepairAmount);
}

void UBTTask_HelperRepair::OnTargetDestroyed(AActor* DestroyedActor)
{
	for (TObjectIterator<UBehaviorTreeComponent> It; It; ++It)
	{
		UBehaviorTreeComponent* BTComp = *It;
		if (!BTComp || !BTComp->IsValidLowLevel()) continue;
		
		uint8* NodeMem = BTComp->GetNodeMemory(this, BTComp->GetActiveInstanceIdx());
		if (!NodeMem) continue;
		
		FBTRepairMemory* MyMemory = reinterpret_cast<FBTRepairMemory*>(NodeMem);
		if (!MyMemory) continue;
		
		if (MyMemory->CachedTarget.Get() == DestroyedActor)
		{
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
			
			MyMemory->bIsTargetDestroyedBound = false;
			MyMemory->CachedTarget = nullptr;
			MyMemory->OwnerComp = nullptr;
			MyMemory->NodeMemoryPtr = nullptr;
			
			FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
		}
	}
}

void UBTTask_HelperRepair::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result)
{
	FBTRepairMemory* MyMemory = reinterpret_cast<FBTRepairMemory*>(NodeMemory);
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
	
	UnbindTargetDestroyedDelegate(NodeMemory);
	
	MyMemory->OwnerComp = nullptr;
	MyMemory->NodeMemoryPtr = nullptr;
	
	FinishLatentTask(OwnerComp, Result);
}

void UBTTask_HelperRepair::UnbindTargetDestroyedDelegate(uint8* NodeMemory)
{
	FBTRepairMemory* MyMemory = reinterpret_cast<FBTRepairMemory*>(NodeMemory);
	if (!MyMemory) return;
	
	if (MyMemory->CachedTarget.IsValid() && MyMemory->bIsTargetDestroyedBound)
	{
		AActor* Target = MyMemory->CachedTarget.Get();
		if (Target)
		{
			Target->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperRepair::OnTargetDestroyed);
		}
		MyMemory->bIsTargetDestroyedBound = false;
	}
}

EBTNodeResult::Type UBTTask_HelperRepair::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Aborted);
	return EBTNodeResult::Aborted;
}