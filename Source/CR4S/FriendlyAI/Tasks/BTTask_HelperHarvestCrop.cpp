#include "BTTask_HelperHarvestCrop.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "AIController.h"
#include "Gimmick/GimmickObjects/Farming/CropsGimmick.h"
#include "../BaseHelperBot.h"
#include "../Controller/HelperBotAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Engine/Engine.h"

UBTTask_HelperHarvestCrop::UBTTask_HelperHarvestCrop()
{
	NodeName = TEXT("HelperHarvestCrop");
	bNotifyTick = true;	
}

uint16 UBTTask_HelperHarvestCrop::GetInstanceMemorySize() const
{
	return sizeof(FBTHarvestCropMemory);
}

void UBTTask_HelperHarvestCrop::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitMemoryType) const
{
	if (InitMemoryType == EBTMemoryInit::Initialize)
	{
		new(NodeMemory) FBTHarvestCropMemory();
	}
}

EBTNodeResult::Type UBTTask_HelperHarvestCrop::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTHarvestCropMemory* MyMemory = reinterpret_cast<FBTHarvestCropMemory*>(NodeMemory);
	if (!MyMemory) return EBTNodeResult::Failed;

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if (!TargetActor) return EBTNodeResult::Failed;

	ACropsGimmick* Crop = Cast<ACropsGimmick>(TargetActor);
	if (!Crop || !Crop->GetIsHarvestable())
	{
		return EBTNodeResult::Failed;
	}
	
	MyMemory->CachedTarget = TargetActor;
	MyMemory->CachedHelper = Pawn;
	
	MyMemory->OwnerComp = &OwnerComp;
	MyMemory->NodeMemoryPtr = NodeMemory;
	
	if (TargetActor && !MyMemory->bIsTargetDestroyedBound)
	{
		TargetActor->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperHarvestCrop::OnTargetDestroyed);
		TargetActor->OnDestroyed.AddDynamic(this, &UBTTask_HelperHarvestCrop::OnTargetDestroyed);
		MyMemory->bIsTargetDestroyedBound = true;
	}

	ABaseHelperBot* Helper = Cast<ABaseHelperBot>(Pawn);
	if (!Helper) return EBTNodeResult::Failed;

	MyMemory->CachedHarvestDuration = Helper->GetHarvestDuration();
	MyMemory->ElapsedTime = 0.f;
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

void UBTTask_HelperHarvestCrop::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FBTHarvestCropMemory* MyMemory = reinterpret_cast<FBTHarvestCropMemory*>(NodeMemory);
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
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}
	
	ACropsGimmick* Crop = Cast<ACropsGimmick>(Target);
	if (!Crop || !Crop->GetIsHarvestable())
	{
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Failed);
		return;
	}

	MyMemory->ElapsedTime += DeltaSeconds;

	if (MyMemory->ElapsedTime >= MyMemory->CachedHarvestDuration)
	{
		Crop->Harvest(Helper);
		CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Succeeded);
	}
}

void UBTTask_HelperHarvestCrop::OnTargetDestroyed(AActor* DestroyedActor)
{
	for (TObjectIterator<UBehaviorTreeComponent> It; It; ++It)
	{
		UBehaviorTreeComponent* BTComp = *It;
		if (!BTComp || !BTComp->IsValidLowLevel()) continue;
		
		uint8* NodeMem = BTComp->GetNodeMemory(this, BTComp->GetActiveInstanceIdx());
		if (!NodeMem) continue;
		
		FBTHarvestCropMemory* MyMemory = reinterpret_cast<FBTHarvestCropMemory*>(NodeMem);
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
			
			FinishLatentTask(*BTComp, EBTNodeResult::Failed);
		}
	}
}

void UBTTask_HelperHarvestCrop::CleanupAndFinish(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type Result)
{
	FBTHarvestCropMemory* MyMemory = reinterpret_cast<FBTHarvestCropMemory*>(NodeMemory);
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

void UBTTask_HelperHarvestCrop::UnbindTargetDestroyedDelegate(uint8* NodeMemory)
{
	FBTHarvestCropMemory* MyMemory = reinterpret_cast<FBTHarvestCropMemory*>(NodeMemory);
	if (!MyMemory) return;
	
	if (MyMemory->CachedTarget.IsValid() && MyMemory->bIsTargetDestroyedBound)
	{
		AActor* Target = MyMemory->CachedTarget.Get();
		if (Target)
		{
			Target->OnDestroyed.RemoveDynamic(this, &UBTTask_HelperHarvestCrop::OnTargetDestroyed);
		}
		MyMemory->bIsTargetDestroyedBound = false;
	}
}

EBTNodeResult::Type UBTTask_HelperHarvestCrop::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CleanupAndFinish(OwnerComp, NodeMemory, EBTNodeResult::Aborted);
	return EBTNodeResult::Aborted;
}