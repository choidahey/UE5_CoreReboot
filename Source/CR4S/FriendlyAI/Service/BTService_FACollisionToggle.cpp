#include "BTService_FACollisionToggle.h"
#include "AIController.h"
#include "FriendlyAI/Controller/FAAIController.h"
#include "FriendlyAI/AnimalFlying.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

UBTService_FACollisionToggle::UBTService_FACollisionToggle()
{
	NodeName = TEXT("Toggle Collision Based On Perch Distance");
}

uint16 UBTService_FACollisionToggle::GetInstanceMemorySize() const
{
	return sizeof(FBTService_FACollisionToggleMemory);
}

void UBTService_FACollisionToggle::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	FBTService_FACollisionToggleMemory* Memory = reinterpret_cast<FBTService_FACollisionToggleMemory*>(NodeMemory);

	AFAAIController* AICon = Cast<AFAAIController>(OwnerComp.GetAIOwner());
	if (!AICon || AICon->FilteredPerchTargets.Num() == 0) return;

	AAnimalFlying* AnimalFlying = Cast<AAnimalFlying>(AICon->GetPawn());
	if (!AnimalFlying) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	AActor* Target = AICon->FilteredPerchTargets[0];
	if (!Target) return;

	const float Distance = AnimalFlying->GetDistanceTo(Target);
	
	if (Distance < 4000.f)
	{
		BB->SetValueAsBool("DoADownTrace", false);
	}
	else if (AnimalFlying->GetDoADownTrace())
	{
		BB->SetValueAsBool("DoADownTrace", true);
	}

	UCapsuleComponent* Capsule = AnimalFlying->GetCapsuleComponent();
	if (!Capsule) return;

	if (Distance < 1000.f)
	{
		if (!Memory->bDoOnceClose)
		{
			//Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Memory->bDoOnceClose = true;
		}
		Memory->bDoOnceFar = false;
	}
	else
	{
		if (AnimalFlying->GetDisableCollisionDuringFlight() && !Memory->bDoOnceFar)
		{
			//Capsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			Memory->bDoOnceFar = true;
		}
		Memory->bDoOnceClose = false;
	}
}