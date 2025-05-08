#include "InteractionComponent.h"

#include "InteractableComponent.h"
#include "Blueprint/UserWidget.h"
#include "CR4S/Gimmick/UI/InteractionWidget.h"

UInteractionComponent::UInteractionComponent()
	: InteractionRange(1000.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitComponent();

	// Start FindProcess
	GetWorld()->GetTimerManager().SetTimer(
		FindProcessTimerHandle,
		this,
		&ThisClass::FindProcess,
		0.1f,
		true);
}

void UInteractionComponent::InitComponent()
{
	OwnerPlayer = Cast<APlayerController>(GetOwner());

	if (IsValid(InteractionWidgetClass))
	{
		InteractionWidgetInstance = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		if (!IsValid(InteractionWidgetInstance))
		{
			UE_LOG(LogTemp, Warning, TEXT("InteractionWidgetInstance is invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionWidgetClass is invalid"));
	}
}

void UInteractionComponent::ShowInteractionWidget() const
{
	if (IsValid(TargetInteractableComponent))
	{
		const FString InteractionText = TargetActor->GetName();
		InteractionWidgetInstance->InitWidget(InteractionText);
		InteractionWidgetInstance->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetInteractableComponent is invalid"));
	}
}

bool UInteractionComponent::TryStartInteraction() const
{
	if (IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryStartInteraction! TargetActor: %s"), *TargetActor->GetName());
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetActor is invalid"));
	return false;
}

void UInteractionComponent::FindProcess()
{
	if (IsValid(OwnerPlayer))
	{
		FVector WorldLocation;
		FVector WorldDirection;
		OwnerPlayer->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

		if (GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			WorldLocation,
			WorldLocation + WorldDirection * InteractionRange,
			ECC_Visibility))
		{
			TargetActor = TraceHitResult.GetActor();

			if (IsValid(TargetActor))
			{
				TargetInteractableComponent = Cast<UInteractableComponent>(TraceHitResult.GetComponent());
				ShowInteractionWidget();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TargetActor is invalid"));
			}
		}
		else
		{
			TargetActor = nullptr;
			TargetInteractableComponent = nullptr;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerPlayer is invalid"));
	}
}
