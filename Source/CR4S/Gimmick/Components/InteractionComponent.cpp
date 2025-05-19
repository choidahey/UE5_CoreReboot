#include "InteractionComponent.h"

#include "InteractableComponent.h"
#include "Blueprint/UserWidget.h"
#include "CR4S/Character/Characters/PlayerCharacter.h"
#include "CR4S/Gimmick/UI/InteractionWidget.h"

UInteractionComponent::UInteractionComponent()
	: InteractionTraceChannel(ECC_GameTraceChannel1)
	  , HitResultTraceDistance(2000.f)
	  , InteractionRange(200.f)
	  , bIsDebugMode(false)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	InitComponent();

	// Start UpdateFindProcess
	GetWorld()->GetTimerManager().SetTimer(
		FindProcessTimerHandle,
		this,
		&ThisClass::UpdateDetectProcess,
		0.1f,
		true);
}

void UInteractionComponent::InitComponent()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (IsValid(PlayerCharacter))
	{
		OwnerPlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

		if (IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->HitResultTraceDistance = HitResultTraceDistance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OwnerPlayerController is invalid"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is invalid"));
	}

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
	if (IsValid(DetectedInteractableComponent))
	{
		if (IsValid(InteractionWidgetInstance))
		{
			InteractionWidgetInstance->InitWidget(DetectedInteractableComponent->GetInteractionText());

			if (!InteractionWidgetInstance->IsInViewport())
			{
				InteractionWidgetInstance->AddToViewport();
			}
		}
	}
}

void UInteractionComponent::HideInteractionWidget() const
{
	if (IsValid(InteractionWidgetInstance))
	{
		InteractionWidgetInstance->RemoveFromParent();
	}
}

bool UInteractionComponent::TryStartInteraction() const
{
	if (IsValid(DetectedActor) && IsValid(DetectedInteractableComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryStartInteraction! TargetActor: %s"), *DetectedActor->GetName());

		DetectedInteractableComponent->TryInteract(OwnerPlayerController);

		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("TargetActor is invalid"));
	return false;
}

void UInteractionComponent::UpdateDetectProcess()
{
	UInteractableComponent* BeforeComponent = DetectedInteractableComponent;

	const bool bIsDetected = TryDetectProcess();

	if (IsValid(BeforeComponent) && (BeforeComponent != DetectedInteractableComponent || !bIsDetected))
	{
		BeforeComponent->DetectionStateChanged(nullptr, false);
	}

	if (bIsDetected && IsValid(DetectedInteractableComponent))
	{
		DetectedInteractableComponent->DetectionStateChanged(OwnerPlayerController, true);
		ShowInteractionWidget();
	}
	else
	{
		HideInteractionWidget();
		DetectedActor = nullptr;
		DetectedInteractableComponent = nullptr;
	}
}

bool UInteractionComponent::TryDetectProcess()
{
	const AActor* OwnerActor = GetOwner();
	if (IsValid(OwnerPlayerController) && IsValid(OwnerActor))
	{
		const FVector OwnerLocation = OwnerActor->GetActorLocation();

		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ScreenCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

		FHitResult HitResult;
		const bool bHit = OwnerPlayerController->GetHitResultAtScreenPosition(
			ScreenCenter, ECC_GameTraceChannel1, false, HitResult);

		DetectedActor = HitResult.GetActor();;
		if (bHit && IsValid(DetectedActor))
		{
			const FVector HitPoint = HitResult.ImpactPoint;
			const float Distance = FVector::DistSquared(OwnerLocation, HitPoint);

			if (bIsDebugMode)
			{
				DrawDebugLine(
					GetWorld(),
					OwnerLocation,
					HitPoint,
					FColor::Green,
					false,
					0.1f
				);
			}

			if (Distance > InteractionRange * InteractionRange)
			{
				return false;
			}

			DetectedInteractableComponent = DetectedActor->FindComponentByClass<UInteractableComponent>();
			if (IsValid(DetectedInteractableComponent))
			{
				return true;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OwnerPlayer is invalid"));
	}

	return false;
}
