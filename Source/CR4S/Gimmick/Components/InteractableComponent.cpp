#include "InteractableComponent.h"

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
