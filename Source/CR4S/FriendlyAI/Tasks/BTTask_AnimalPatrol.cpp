#include "BTTask_AnimalPatrol.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "FriendlyAI/BaseAnimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_AnimalPatrol::UBTTask_AnimalPatrol()
{
   NodeName = TEXT("Animal Patrol");
   bNotifyTick = true;
   bCreateNodeInstance = true;
}

uint16 UBTTask_AnimalPatrol::GetInstanceMemorySize() const
{
   return sizeof(FPatrolMemory);
}

EBTNodeResult::Type UBTTask_AnimalPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
   if (!IsValid(Pawn))
   {
       return EBTNodeResult::Failed;
   }

   FPatrolMemory* Mem = reinterpret_cast<FPatrolMemory*>(NodeMemory);
   Mem->ElapsedTime = 0.f;
   Mem->CurrentSpeed = 0.f;
   Mem->bSwitched = false;
    
   FVector Origin = Pawn->GetActorLocation();
   UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
   if (!NavSys)
   {
       return EBTNodeResult::Failed;
   }

   FNavLocation RandomPoint;
   if (!NavSys->GetRandomPointInNavigableRadius(Origin, PatrolRadius, RandomPoint))
   {
       return EBTNodeResult::Failed;
   }

   Mem->PatrolDestination = RandomPoint.Location;
   Mem->TotalDistance = FVector::Dist(Origin, Mem->PatrolDestination);
   Mem->Pattern = FMath::RandRange(0, 2);

   ABaseAnimal* Animal = Cast<ABaseAnimal>(Pawn);
   if (!Animal)
   {
       return EBTNodeResult::Failed;
   }
    
   if (UCharacterMovementComponent* CharMove = Animal->GetCharacterMovement())
   {
       CharMove->MaxWalkSpeed = Mem->CurrentSpeed;
   }
   else
   {
       return EBTNodeResult::Failed;
   }
    
    if (AAIController* AICon = OwnerComp.GetAIOwner())
    {
        FAIMoveRequest MoveRequest;
        MoveRequest.SetGoalLocation(Mem->PatrolDestination);
        MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
        MoveRequest.SetUsePathfinding(true);
        MoveRequest.SetAllowPartialPath(false);
        MoveRequest.SetRequireNavigableEndLocation(true);
        MoveRequest.SetReachTestIncludesAgentRadius(false);
    
        FNavPathSharedPtr NavPath;
        AICon->MoveTo(MoveRequest, &NavPath);
    }
   else
   {
       return EBTNodeResult::Failed;
   }

   return EBTNodeResult::InProgress;
}

void UBTTask_AnimalPatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
   APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
   if (!IsValid(Pawn))
   {
       FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
       return;
   }

   FPatrolMemory* Mem = reinterpret_cast<FPatrolMemory*>(NodeMemory);
   Mem->ElapsedTime += DeltaSeconds;
   
   if (Mem->ElapsedTime >= MaxWaitTime)
   {
       FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
       return;
   }

   ABaseAnimal* Animal = Cast<ABaseAnimal>(Pawn);
   if (!Animal)
   {
       return;
   }

   FVector CurrentLocation = Pawn->GetActorLocation();
   float CurrentDistance = FVector::Dist(CurrentLocation, Mem->PatrolDestination);
   float Progress = Mem->TotalDistance > 0.f ? FMath::Clamp(1.f - (CurrentDistance / Mem->TotalDistance), 0.f, 1.f) : 1.f;
   
   float WalkSpeed = Animal->GetCurrentStats().WalkSpeed;
   float RunSpeed = Animal->GetCurrentStats().RunSpeed;
   float TargetSpeed = 0.f;
    
   FString MoveStatusString = TEXT("Unknown");
   FString PathFollowingString = TEXT("Unknown");
   bool bHasValidPath = false;
   float RemainingPathDistance = 0.f;
   
   if (AAIController* AICon = OwnerComp.GetAIOwner())
   {
       EPathFollowingStatus::Type MoveStatus = AICon->GetMoveStatus();
       switch (MoveStatus)
       {
       case EPathFollowingStatus::Idle: MoveStatusString = TEXT("Idle"); break;
       case EPathFollowingStatus::Waiting: MoveStatusString = TEXT("Waiting"); break;
       case EPathFollowingStatus::Paused: MoveStatusString = TEXT("Paused"); break;
       case EPathFollowingStatus::Moving: MoveStatusString = TEXT("Moving"); break;
       }
       
       if (UPathFollowingComponent* PFC = AICon->GetPathFollowingComponent())
       {
           bHasValidPath = PFC->HasValidPath();
           if (bHasValidPath)
           {
               RemainingPathDistance = PFC->GetRemainingPathCost();
               PathFollowingString = FString::Printf(TEXT("Valid(Remaining: %f)"), RemainingPathDistance);
           }
           else
           {
               PathFollowingString = TEXT("Invalid");
           }
       }
   }
    
   switch (Mem->Pattern)
   {
   case 0:
       TargetSpeed = WalkSpeed;
       break;
   
   case 1:
       TargetSpeed = RunSpeed;
       break;
   
   case 2:
       if (Progress <= 0.5f)
       {
           TargetSpeed = WalkSpeed;
       }
       else
       {
           if (!Mem->bSwitched)
           {
               Mem->bSwitched = true;
           }
           TargetSpeed = RunSpeed;
       }
       break;
   }
    
    if (CurrentDistance <= DecelerationDistance)
    {
        float MinSpeed = FMath::Max(30.f, CurrentDistance * 0.3f);
        float DecelerationRatio = FMath::Clamp(CurrentDistance / DecelerationDistance, 0.3f, 1.0f);
        TargetSpeed = FMath::Max(TargetSpeed * DecelerationRatio, MinSpeed);
    }
    
   Mem->CurrentSpeed = FMath::FInterpTo(Mem->CurrentSpeed, TargetSpeed, DeltaSeconds, SpeedInterpSpeed);
   
   if (UCharacterMovementComponent* CharMove = Animal->GetCharacterMovement())
   {
       float OldMaxWalkSpeed = CharMove->MaxWalkSpeed;
       CharMove->MaxWalkSpeed = Mem->CurrentSpeed;
   }
    
    if (CurrentDistance <= AcceptanceRadius || (MoveStatusString == TEXT("Idle") && CurrentDistance <= AcceptanceRadius * 1.5f))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }
    
    if (FMath::Fmod(Mem->ElapsedTime, 2.0f) < DeltaSeconds && MoveStatusString == TEXT("Idle"))
    {
        if (AAIController* AICon = OwnerComp.GetAIOwner())
        {
            if (CurrentDistance <= AcceptanceRadius)
            {
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
                return;
            }
        
            AICon->MoveToLocation(Mem->PatrolDestination, AcceptanceRadius);
        }
    }
}

EBTNodeResult::Type UBTTask_AnimalPatrol::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
   APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
   if (IsValid(Pawn))
   {
       ABaseAnimal* Animal = Cast<ABaseAnimal>(Pawn);
       if (Animal && Animal->GetCharacterMovement())
       {
           Animal->GetCharacterMovement()->MaxWalkSpeed = Animal->GetCurrentStats().WalkSpeed;
       }
   }
   return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_AnimalPatrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
   APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
   if (IsValid(Pawn))
   {
       ABaseAnimal* Animal = Cast<ABaseAnimal>(Pawn);
       if (Animal && Animal->GetCharacterMovement())
       {
           Animal->GetCharacterMovement()->MaxWalkSpeed = Animal->GetCurrentStats().WalkSpeed;
       }
   }
        
   Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}