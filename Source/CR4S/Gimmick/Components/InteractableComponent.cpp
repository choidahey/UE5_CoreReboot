#include "InteractableComponent.h"

#include "Gimmick/GimmickObjects/BaseGimmick/BaseGimmick.h"

UInteractableComponent::UInteractableComponent()
	: DefaultHighlightColor(FColor::Green)
{
	PrimaryComponentTick.bCanEverTick = false;

	InteractionText = NSLOCTEXT("InteractableComponent", "InteractionText", "Default Interaction Text");

	HighlightOpacityParamName = TEXT("HighlightOpacity");
	HighlightColorParamName = TEXT("HighlightColor");
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	InitHighlightMaterial();
}

void UInteractableComponent::TryInteract(const APlayerController* PlayerController) const
{
	if (IsValid(PlayerController))
	{
		OnTryInteract.ExecuteIfBound();
		OnChangeCharacter.ExecuteIfBound(PlayerController);
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
