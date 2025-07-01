#include "BTTask_HelperBotBMoveToTarget.h"
#include "../Component/AIJumpComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "../Component/AIJumpComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_HelperBotBMoveToTarget::UBTTask_HelperBotBMoveToTarget()
{
	NodeName = TEXT("HelperBot tt");
}

uint16 UBTTask_HelperBotBMoveToTarget::GetInstanceMemorySize() const
{
	return sizeof(FBTMoveToTargetMemory);
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTMoveToTargetMemory* MyMemory = reinterpret_cast<FBTMoveToTargetMemory*>(NodeMemory);

	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	MyMemory->OwnerCompPtr = &OwnerComp;

	UAIJumpComponent* JumpComp = Pawn->FindComponentByClass<UAIJumpComponent>();
	if (JumpComp)
	{
		MyMemory->JumpComponent = JumpComp;
	}
	
	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		return EBTNodeResult::Failed;
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsObject("TargetActor", PlayerPawn);
	}
	
	FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UBTTask_HelperBotBMoveToTarget::CheckDistanceAndRotation, MyMemory);
	MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().SetTimer(MyMemory->CheckTimer, TimerDelegate, 0.1f, true);

	return EBTNodeResult::InProgress;
}

void UBTTask_HelperBotBMoveToTarget::CheckDistanceAndRotation(FBTMoveToTargetMemory* MyMemory)
{
	if (!MyMemory || !MyMemory->OwnerCompPtr)
		return;

	AAIController* AICon = MyMemory->OwnerCompPtr->GetAIOwner();
	if (!AICon)
		return;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
		return;

	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
		return;

	const float Distance = FVector::Dist(Pawn->GetActorLocation(), PlayerPawn->GetActorLocation());

	if (MyMemory->JumpComponent)
	{
		if (Distance >= DistanceThreshold)
		{
			if (!MyMemory->JumpComponent->GetIsComponentActive())
			{
				MyMemory->JumpComponent->ActivateJumpComponent();
			}
			if (MyMemory->RotationTimer.IsValid())
			{
				MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->RotationTimer);
			}
		}
		else
		{
			if (MyMemory->JumpComponent->GetIsComponentActive())
			{
				MyMemory->JumpComponent->DeactivateJumpComponent();
			}
			if (!MyMemory->RotationTimer.IsValid())
			{
				FTimerDelegate RotationDelegate = FTimerDelegate::CreateUObject(this, &UBTTask_HelperBotBMoveToTarget::HandleTargetRotation, MyMemory);
				MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().SetTimer(MyMemory->RotationTimer, RotationDelegate, 0.016f, true);
			}
		}
	}
}

void UBTTask_HelperBotBMoveToTarget::HandleTargetRotation(FBTMoveToTargetMemory* MyMemory)
{
	if (!MyMemory || !MyMemory->OwnerCompPtr)
		return;

	AAIController* AICon = MyMemory->OwnerCompPtr->GetAIOwner();
	if (!AICon)
		return;

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
		return;

	AActor* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
		return;

	FVector TargetDirection = (PlayerPawn->GetActorLocation() - Pawn->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = TargetDirection.Rotation();
	FRotator CurrentRotation = Pawn->GetActorRotation();
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, 0.016f, 270.0f);
	Pawn->SetActorRotation(NewRotation);
}

EBTNodeResult::Type UBTTask_HelperBotBMoveToTarget::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FBTMoveToTargetMemory* MyMemory = reinterpret_cast<FBTMoveToTargetMemory*>(NodeMemory);

	if (MyMemory->OwnerCompPtr)
	{
		MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->CheckTimer);
		MyMemory->OwnerCompPtr->GetWorld()->GetTimerManager().ClearTimer(MyMemory->RotationTimer);
	}

	if (MyMemory->JumpComponent)
	{
		MyMemory->JumpComponent->DeactivateJumpComponent();
		MyMemory->JumpComponent = nullptr;
	}

	MyMemory->OwnerCompPtr = nullptr;
	return EBTNodeResult::Aborted;
}