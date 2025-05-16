#include "InteractableComponent.h"

#include "Gimmick/GimmickObjects/BaseGimmick.h"

UInteractableComponent::UInteractableComponent()
	: InteractionTraceChannel(ECC_GameTraceChannel1)
	  , DefaultHighlightColor(FColor::Green)
{
	PrimaryComponentTick.bCanEverTick = false;
	
	InteractionText = NSLOCTEXT("InteractableComponent", "InteractionText", "Default Interaction Text");

	HighlightOpacityParamName = TEXT("HighlightOpacity");
	HighlightColorParamName = TEXT("HighlightColor");
}

#if WITH_EDITOR
void UInteractableComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	UpdateTraceBlocking();
}

void UInteractableComponent::PostLoad()
{
	Super::PostLoad();
	UpdateTraceBlocking();
	
}

void UInteractableComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateTraceBlocking();
	
}
#endif

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	InitHighlightMaterial();
}

void UInteractableComponent::UpdateTraceBlocking() const
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
				MeshComp->SetCollisionResponseToChannel(InteractionTraceChannel, ECR_Block);
			}
		}
	}
}


void UInteractableComponent::TryInteract(const APlayerController* PlayerController) const
{
	if (IsValid(PlayerController))
	{
		OnTryInteract.ExecuteIfBound();
	}
}

void UInteractableComponent::InitHighlightMaterial()
{
	const ABaseGimmick* OwnerGimmick = Cast<ABaseGimmick>(GetOwner());
	if (IsValid(OwnerGimmick))
	{
		UStaticMeshComponent* StaticMeshComponent = OwnerGimmick->GetGimmickMeshComponent();
		if (IsValid(StaticMeshComponent))
		{
			HighlightMaterialInstance = UMaterialInstanceDynamic::Create(HighlightMaterial, this);
			StaticMeshComponent->SetOverlayMaterial(HighlightMaterialInstance);

			SetHighlight(false);
			ChangeHighlightColor(DefaultHighlightColor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StaticMeshComponent is invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerGimmick is invalid"));
	}
}

void UInteractableComponent::SetHighlight(const bool bIsDetected) const
{
	if (IsValid(HighlightMaterialInstance))
	{
		const float Opacity = bIsDetected ? 1.f : 0.f;
		HighlightMaterialInstance->SetScalarParameterValue(HighlightOpacityParamName, Opacity);
	}
}

void UInteractableComponent::ChangeHighlightColor(const FColor& InHighlightColor) const
{
	if (IsValid(HighlightMaterialInstance))
	{
		HighlightMaterialInstance->SetVectorParameterValue(HighlightColorParamName, InHighlightColor);
	}
}
