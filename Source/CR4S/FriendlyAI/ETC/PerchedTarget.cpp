#include "PerchedTarget.h"
#include "Components/SphereComponent.h"
#include "FriendlyAI/AnimalFlying.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

APerchedTarget::APerchedTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = Sphere;
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &APerchedTarget::OnOverlapBegin);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &APerchedTarget::OnOverlapEnd);
}

void APerchedTarget::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AAnimalFlying* Bird = Cast<AAnimalFlying>(OtherActor);
	if (!Bird) return;

	UBlackboardComponent* BB = Bird->BlackboardComponent;
	if (!BB) return;

	if (bOccupied)
	{
		if (CurrentOccupant != Bird)
		{
			BB->SetValueAsBool("IsLookingForPerch", false);
			BB->SetValueAsBool("FlyAway", true);
		}
	}
	else
	{
		if (BB->GetValueAsBool("IsLookingForPerch"))
		{
			SetOccupied(true);
			CurrentOccupant = Bird;
		}
		else
		{
			BB->SetValueAsBool("FlyAway", true);
		}
	}
}

void APerchedTarget::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AAnimalFlying* Bird = Cast<AAnimalFlying>(OtherActor);
	if (!Bird) return;
	
	if (Bird == CurrentOccupant)
	{
		UBlackboardComponent* BB = Bird->BlackboardComponent;
		if (BB)
		{
			BB->SetValueAsBool("AllPerchesAreFull", false);
		}
		
		SetOccupied(false);
		CurrentOccupant = nullptr;
	}
}