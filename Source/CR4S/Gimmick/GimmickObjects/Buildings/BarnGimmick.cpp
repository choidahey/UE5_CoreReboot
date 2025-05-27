#include "BarnGimmick.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "../../UI/BarnManagementWidget.h"
#include "Blueprint/UserWidget.h"
#include "../../../FriendlyAI/BaseAnimal.h"

ABarnGimmick::ABarnGimmick()
{
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	BarnMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarnMeshComponent"));
	BarnMeshComponent->SetupAttachment(RootComponent);
}

void ABarnGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractableComponent))
	{
		InteractableComponent->OnTryInteract.BindDynamic(this, &ABarnGimmick::OnInteract);
	}
}

void ABarnGimmick::OnInteract(AActor* Interactor)
{
	if (!IsValid(BarnUIInstance) && IsValid(BarnUIClass))
	{
		BarnUIInstance = CreateWidget<UBarnManagementWidget>(GetWorld(), BarnUIClass);
		if (IsValid(BarnUIInstance))
		{
			BarnUIInstance->OwningGimmick = this;
			BarnUIInstance->AddToViewport();
		}
	}
}

void ABarnGimmick::SpawnAnimalByRowName(FName RowName)
{
	if (TSubclassOf<AActor>* FoundClass = AnimalBlueprintMap.Find(RowName))
	{
		if (IsValid(*FoundClass))
		{
			FVector SpawnLocation = GetActorLocation() + FVector(100.f, 0.f, 150.f);
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			if (AActor* Spawned = GetWorld()->SpawnActor<AActor>(*FoundClass, SpawnLocation, FRotator::ZeroRotator, Params))
			{
				if (ABaseAnimal* Animal = Cast<ABaseAnimal>(Spawned))
				{
					Animal->SetbIsTamed(true);
				}
			}
		}
	}
}
