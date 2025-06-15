#include "InteractionComponent.h"

#include "CR4S.h"
#include "InteractableComponent.h"
#include "Blueprint/UserWidget.h"
#include "CR4S/Character/Characters/PlayerCharacter.h"
#include "CR4S/Gimmick/UI/InteractionWidget.h"
#include "UI/InGame/SurvivalHUD.h"

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

	StartDetectProcess();
}

void UInteractionComponent::InitComponent()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());

	if (CR4S_VALIDATE(LogGimmick, IsValid(PlayerCharacter)))
	{
		OwnerPlayerController = Cast<APlayerController>(PlayerCharacter->GetController());

		if (CR4S_VALIDATE(LogGimmick, IsValid(OwnerPlayerController)))
		{
			OwnerPlayerController->HitResultTraceDistance = HitResultTraceDistance;
			SurvivalHUD = Cast<ASurvivalHUD>(OwnerPlayerController->GetHUD());
		}
	}

	if (!CR4S_VALIDATE(LogGimmick, IsValid(InteractionWidgetClass)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(SurvivalHUD)))
	{
		return;
	}

	InteractionWidgetInstance = SurvivalHUD->CreateAndAddWidget(InteractionWidgetClass, 0, ESlateVisibility::Collapsed);
}

void UInteractionComponent::StartDetectProcess()
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractionWidgetInstance)))
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(FindProcessTimerHandle))
		{
			return;
		}
		
		// Start the UpdateDetectProcess
		GetWorld()->GetTimerManager().SetTimer(
			FindProcessTimerHandle,
			this,
			&ThisClass::UpdateDetectProcess,
			0.1f,
			true);
	}
}

void UInteractionComponent::StopDetectProcess()
{
	GetWorld()->GetTimerManager().ClearTimer(FindProcessTimerHandle);

	HideInteractionWidget();

	if (IsValid(DetectedInteractableComponent))
	{
		DetectedInteractableComponent->DetectionStateChanged(nullptr, false);
		DetectedInteractableComponent = nullptr;
	}
}

void UInteractionComponent::ShowInteractionWidget() const
{
	if (!IsValidWidgetRef() ||
		!CR4S_VALIDATE(LogGimmick, IsValid(DetectedInteractableComponent)))
	{
		return;
	}

	InteractionWidgetInstance->SetInteractionText(DetectedInteractableComponent->GetInteractionText());

	if (InteractionWidgetInstance->GetVisibility() == ESlateVisibility::Collapsed)
	{
		SurvivalHUD->ToggleWidget(InteractionWidgetInstance);
	}
}

void UInteractionComponent::HideInteractionWidget() const
{
	if (!IsValidWidgetRef() ||
		InteractionWidgetInstance->GetVisibility() != ESlateVisibility::Visible)
	{
		return;
	}

	SurvivalHUD->ToggleWidget(InteractionWidgetInstance);
}

bool UInteractionComponent::TryStartInteraction() const
{
	if (!IsValid(DetectedActor) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(DetectedInteractableComponent)))
	{
		return false;
	}

	// CR4S_Log(LogGimmick, Warning, TEXT("TryStartInteraction! TargetActor: %s"), *DetectedActor->GetName());
	DetectedInteractableComponent->TryInteract(GetOwner());

	return true;
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
		DetectedInteractableComponent->DetectionStateChanged(GetOwner(), true);
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

	if (!CR4S_VALIDATE(LogGimmick, IsValid(OwnerActor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(OwnerPlayerController)))
	{
		GetWorld()->GetTimerManager().ClearTimer(FindProcessTimerHandle);
		return false;
	}

	const FVector OwnerLocation = OwnerActor->GetActorLocation();

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ScreenCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

	FHitResult HitResult;
	const bool bHit = OwnerPlayerController->GetHitResultAtScreenPosition(
		ScreenCenter, InteractionTraceChannel, false, HitResult);

	DetectedActor = HitResult.GetActor();
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

	return false;
}

bool UInteractionComponent::IsValidWidgetRef() const
{
	return CR4S_VALIDATE(LogGimmick, IsValid(SurvivalHUD)) &&
		CR4S_VALIDATE(LogGimmick, IsValid(InteractionWidgetInstance));
}
