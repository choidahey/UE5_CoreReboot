#include "InteractableComponent.h"

#include "CR4S.h"

UInteractableComponent::UInteractableComponent()
	: InteractionTraceChannel(ECC_GameTraceChannel1),
	  DefaultHighlightColor(FColor::Green)
{
	PrimaryComponentTick.bCanEverTick = false;

	InteractionText = FText::FromString(TEXT("Default Interaction Text"));

	HighlightOpacityParamName = TEXT("HighlightOpacity");
	HighlightColorParamName = TEXT("HighlightColor");
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	UpdateTraceBlocking();

	InitHighlightMaterial();
}

void UInteractableComponent::UpdateTraceBlocking(const ECollisionResponse NewResponse) const
{
	const AActor* Owner = GetOwner();
	if (IsValid(Owner))
	{
		TArray<UMeshComponent*> MeshComponents;
		Owner->GetComponents<UMeshComponent>(MeshComponents);
		for (UMeshComponent* MeshComp : MeshComponents)
		{
			if (IsValid(MeshComp))
			{
				MeshComp->SetCollisionResponseToChannel(InteractionTraceChannel, NewResponse);
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

void UInteractableComponent::InitHighlightMaterial()
{
	const AActor* Owner = GetOwner();

	if (!CR4S_VALIDATE(LogGimmick, IsValid(Owner)))
	{
		return;
	}

	TArray<UMeshComponent*> MeshComponents;
	Owner->GetComponents<UMeshComponent>(MeshComponents);
	HighlightMaterialInstance = UMaterialInstanceDynamic::Create(HighlightMaterial, this);

	if (!CR4S_VALIDATE(LogGimmick, IsValid(HighlightMaterialInstance)))
	{
		return;
	}

	SetHighlight(false);
	ChangeHighlightColor(DefaultHighlightColor);

	for (UMeshComponent* MeshComp : MeshComponents)
	{
		if (IsValid(MeshComp))
		{
			MeshComp->SetOverlayMaterial(HighlightMaterialInstance);
		}
	}
}

void UInteractableComponent::SetHighlight(const bool bIsDetected) const
{
	const float Opacity = bIsDetected ? 1.f : 0.f;
	HighlightMaterialInstance->SetScalarParameterValue(HighlightOpacityParamName, Opacity);
}

void UInteractableComponent::ChangeHighlightColor(const FColor& InHighlightColor) const
{
	HighlightMaterialInstance->SetVectorParameterValue(HighlightColorParamName, InHighlightColor);
}
