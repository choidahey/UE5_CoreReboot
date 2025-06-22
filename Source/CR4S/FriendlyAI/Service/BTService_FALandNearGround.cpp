#include "BTService_FALandNearGround.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "FriendlyAI/AnimalFlying.h"
#include "Kismet/GameplayStatics.h"

struct FBTService_FALandNearGroundMemory
{
    bool bHasLanded = false;
};

UBTService_FALandNearGround::UBTService_FALandNearGround()
{
    NodeName = TEXT("Land When Close To Ground Target1");
}

void UBTService_FALandNearGround::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    
    FBTService_FALandNearGroundMemory* MyMemory = reinterpret_cast<FBTService_FALandNearGroundMemory*>(NodeMemory);
    if (!MyMemory) return;

    if (MyMemory->bHasLanded) return;

    AFAAIController* AICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
    if (!AICon) return;

    APawn* ControlledPawn = AICon->GetPawn();
    if (!ControlledPawn) return;

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp) return;

    if (AICon->FilteredGroundTargets.Num() == 0) return;

    AActor* GroundTarget = AICon->FilteredGroundTargets[0];
    if (!GroundTarget) return;

    float Distance = ControlledPawn->GetDistanceTo(GroundTarget);
    if (Distance > 2000.f)
    {
       MyMemory->bHasLanded = false;
       return;
    }  
    MyMemory->bHasLanded = true;

    AAnimalFlying* FlyingAnimal = Cast<AAnimalFlying>(ControlledPawn);
    if (!FlyingAnimal) return;
    
    UCapsuleComponent* Capsule = FlyingAnimal->GetCapsuleComponent();
    if (Capsule)
    {
        Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        Capsule->SetCollisionProfileName(TEXT("Pawn"));
    }
    
    FVector Forward = ControlledPawn->GetActorForwardVector() * 700.f;
    FVector Up = ControlledPawn->GetActorUpVector() * 400.f;
    FVector CurrentVelocity = FlyingAnimal->GetCharacterMovement()->Velocity;
    FVector FinalVelocity = CurrentVelocity + Forward + Up;
    
    UCharacterMovementComponent* MoveComp = FlyingAnimal->GetCharacterMovement();
    if (MoveComp)
    {
        MoveComp->SetMovementMode(MOVE_NavWalking);
        MoveComp->GravityScale = 0.1f;
        MoveComp->Velocity = FinalVelocity;
    }
    
    FlyingAnimal->SetActorEnableCollision(true);
    
    BlackboardComp->SetValueAsBool("AreWePerched", false);
    BlackboardComp->SetValueAsBool("AreWeFlying", false);
    BlackboardComp->SetValueAsBool("TimeToLand", false);
    BlackboardComp->SetValueAsBool("AreWeWalking", true);
}