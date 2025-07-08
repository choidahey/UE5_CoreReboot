#include "InteractableComponent.h"
#include "Components/SphereComponent.h" 
#include "CR4S.h"

UInteractableComponent::UInteractableComponent()
	: InteractionTraceChannel(ECC_GameTraceChannel1)
{
	PrimaryComponentTick.bCanEverTick = false;

	InteractionText = FText::FromString(TEXT("Default Interaction Text"));
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateTraceBlocking();
}

void UInteractableComponent::UpdateTraceBlocking(const ECollisionResponse NewResponse)
{
	const AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		Owner->GetComponents<UMeshComponent>(OwnerMeshComponents);
		for (UMeshComponent* MeshComp : OwnerMeshComponents)
		{
			if (IsValid(MeshComp))
			{
				MeshComp->SetCollisionResponseToChannel(InteractionTraceChannel, NewResponse);
				MeshComp->SetRenderCustomDepth(true);
				MeshComp->SetCustomDepthStencilValue(0);
			}
		}
	}
}


void UInteractableComponent::TryInteract(AActor* Interactor) const
{
	if (OnTryInteract.IsBound())
	{
		OnTryInteract.Broadcast(Interactor);
	}
}

void UInteractableComponent::DetectionStateChanged(AActor* DetectingActor,
                                                   const bool bIsDetected) const
{
	SetHighlight(bIsDetected);

	if (OnDetectionStateChanged.IsBound())
	{
		OnDetectionStateChanged.Broadcast(DetectingActor, bIsDetected);
	}
}

void UInteractableComponent::ActivateEndOverlapCollision()
{
	if (!bAllowEndOverlap) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (Owner->FindComponentByClass<USphereComponent>()) return;

	USphereComponent* Sphere = NewObject<USphereComponent>(Owner, TEXT("EndOverlapSphere"));
	if (!Sphere) return;

	Sphere->InitSphereRadius(EndOverlapRadius);
	Sphere->SetCollisionProfileName(TEXT("Trigger"));
	Sphere->SetupAttachment(Owner->GetRootComponent());
	Sphere->SetGenerateOverlapEvents(true);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnEndOverlap);

	Sphere->RegisterComponent();

#if WITH_EDITOR
	if (bDrawDebugSphere)
	{
		DrawDebugSphere(
			GetWorld(),
			Sphere->GetComponentLocation(),
			EndOverlapRadius,
			16,
			FColor::Green,
			true,
			10.f,
			0,
			2.0f
		);
	}
#endif
}

void UInteractableComponent::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OnEndSphereOverlap.IsBound())
	{
		OnEndSphereOverlap.Broadcast(OtherActor);
	}
}

void UInteractableComponent::SetHighlight(const bool bIsDetected) const
{
	const int32 StencilValue = bIsDetected ? 1 : 0;
	
	for (UMeshComponent* MeshComp : OwnerMeshComponents)
	{
		if (IsValid(MeshComp))
		{
			MeshComp->SetRenderCustomDepth(true);
			MeshComp->SetCustomDepthStencilValue(StencilValue);
		}
	}
}
