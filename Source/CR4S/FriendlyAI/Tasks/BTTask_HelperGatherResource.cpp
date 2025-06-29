#include "BTTask_HelperGatherResource.h" 
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../../Gimmick/Components/DestructibleComponent.h"
#include "../../Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "../BaseHelperBot.h"
#include "../../Inventory/Components/PlayerInventoryComponent.h"
#include "Gimmick/GimmickObjects/ResourceGimmick/TreeGimmick.h"
#include "../Controller/HelperBotAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/Engine.h"

UBTTask_HelperGatherResource::UBTTask_HelperGatherResource()
{
	NodeName = TEXT("HelperMining");
	bNotifyTick = true;
}

uint16 UBTTask_HelperGatherResource::GetInstanceMemorySize() const
{
	return sizeof(FBTGatherResourceMemory);
}

void UBTTask_HelperGatherResource::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitMemoryType) const
{
	if (InitMemoryType == EBTMemoryInit::Initialize)
	{
		new(NodeMemory) FBTGatherResourceMemory();
	}
}

EBTNodeResult::Type UBTTask_HelperGatherResource::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTGatherResourceMemory* MyMemory = reinterpret_cast<FBTGatherResourceMemory*>(NodeMemory);
	if (!MyMemory) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	if (!TargetActor->IsA(ADestructibleGimmick::StaticClass()))
	{
		return EBTNodeResult::Failed;
	}
	
	MyMemory->CachedTarget = TargetActor;
	MyMemory->CachedHelper = Pawn;
	
	MyMemory->OwnerComp = &OwnerComp;
	MyMemory->NodeMemoryPtr = NodeMemory;
	
	if (TargetActor && !MyMemory->bIsTargetDestroyedBound)
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
		TargetActor->OnDestroyed.AddDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
		MyMemory->bIsTargetDestroyedBound = true;
	}

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

	static int32 HelperCounter = 0;
	static TMap<AActor*, int32> HelperIndexMap;

	if (!HelperIndexMap.Contains(Helper))
	{
		HelperIndexMap.Add(Helper, HelperCounter++);
	}

	int32 MyIndex = HelperIndexMap[Helper];
	float InitialDelay = (MyIndex % 1000) * 0.001f;
	
	GetWorld()->GetTimerManager().SetTimer(
		MyMemory->DamageTimerHandle,
		FTimerDelegate::CreateLambda([MyMemory, Helper]()
		{
			if (MyMemory->CachedTarget.IsValid() && MyMemory->CachedHelper.IsValid() && !MyMemory->CachedTarget->IsActorBeingDestroyed())
			{
				float DamagePerTick = MyMemory->CachedDamagePerSecond * 0.2f;
				UGameplayStatics::ApplyDamage(
					MyMemory->CachedTarget.Get(), 
					DamagePerTick, 
					Helper->GetController(), 
					Helper, 
					UDamageType::StaticClass()
				);
			}
		}),
		0.2f,
		true,
		InitialDelay
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_HelperGatherResource::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTGatherResourceMemory* MyMemory = reinterpret_cast<FBTGatherResourceMemory*>(NodeMemory);
	if (!MyMemory || !MyMemory->CachedTarget.IsValid() || !MyMemory->CachedHelper.IsValid())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}
	
	if (MyMemory->CachedTarget->IsActorBeingDestroyed())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
		return;
	}
}

void UBTTask_HelperGatherResource::OnTargetDestroyed(AActor* DestroyedActor)
{
	for (TObjectIterator<UBehaviorTreeComponent> It; It; ++It)
	{
		UBehaviorTreeComponent* BTComp = *It;
		if (!BTComp || !BTComp->IsValidLowLevel()) continue;
		
		uint8* NodeMem = BTComp->GetNodeMemory(this, BTComp->GetActiveInstanceIdx());
		if (!NodeMem) continue;
		
		FBTGatherResourceMemory* MyMemory = reinterpret_cast<FBTGatherResourceMemory*>(NodeMem);
		if (!MyMemory) continue;
		
		if (MyMemory->CachedTarget.Get() == DestroyedActor)
		{
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

void UBTTask_HelperGatherResource::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result)
{
	FBTGatherResourceMemory* MyMemory = reinterpret_cast<FBTGatherResourceMemory*>(NodeMemory);
	if (!MyMemory)
	{
		FinishLatentTask(OwnerComp, Result);
		return;
	}
	
	if (MyMemory->DamageTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(MyMemory->DamageTimerHandle);
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

void UBTTask_HelperGatherResource::UnbindTargetDestroyedDelegate(uint8* NodeMemory)
{
	FBTGatherResourceMemory* MyMemory = reinterpret_cast<FBTGatherResourceMemory*>(NodeMemory);
	if (!MyMemory) return;
	
	if (MyMemory->CachedTarget.IsValid() && MyMemory->bIsTargetDestroyedBound)
	{
		AActor* Target = MyMemory->CachedTarget.Get();
		if (Target)
		{
			Target->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperGatherResource::OnTargetDestroyed);
		}
		MyMemory->bIsTargetDestroyedBound = false;
	}
}

EBTNodeResult::Type UBTTask_HelperGatherResource::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Aborted);
	return EBTNodeResult::Aborted;
}