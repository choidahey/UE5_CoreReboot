#include "InteractableComponent.h"
#include "Components/SphereComponent.h" 
#include "CR4S.h"

UInteractableComponent::UInteractableComponent()
	: InteractionTraceChannel(ECC_GameTraceChannel1),
	  DefaultHighlightColor(FLinearColor::Green)
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

void UInteractableComponent::ChangeHighlightColor(const FLinearColor& InHighlightColor)
{
	HighlightMaterialInstance->SetVectorParameterValue(HighlightColorParamName, InHighlightColor);
}

FLinearColor UInteractableComponent::GetHighlightColor() const
{
	if (IsValid(HighlightMaterialInstance))
	{
		FLinearColor CurrentColor;
		HighlightMaterialInstance->GetVectorParameterValue(FMaterialParameterInfo(HighlightColorParamName), CurrentColor);
		return CurrentColor;
	}

	return DefaultHighlightColor;
}
