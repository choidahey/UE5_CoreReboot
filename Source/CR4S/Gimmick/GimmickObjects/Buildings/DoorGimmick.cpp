#include "DoorGimmick.h"

#include "Gimmick/Components/InteractableComponent.h"

ADoorGimmick::ADoorGimmick()
	: bIsMoving(false)
	  , bIsOpen(false)
	  , bNextStateIsOpen(false)
	  , OpenAngle(90.f)
	  , DoorSpeed(90.f)
	  , ClosedRotation(FRotator::ZeroRotator)
	  , OpenRotation(FRotator::ZeroRotator)
	  , TargetRotation()
	  , InteractionTextOpen(NSLOCTEXT("Gimmick", "Door_Open", "Open"))
	  , InteractionTextClose(NSLOCTEXT("Gimmick", "Door_Close", "Close"))
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADoorGimmick::BeginPlay()
{
	Super::BeginPlay();

	ClosedRotation = GetActorRotation();
	OpenRotation = FRotator(ClosedRotation.Pitch, ClosedRotation.Yaw + OpenAngle, ClosedRotation.Roll);

	UpdateInteractionText();
}

void ADoorGimmick::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsMoving)
	{
		const FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaSeconds, DoorSpeed);
		NewRotation.Normalize();
		SetActorRotation(NewRotation);

		if (NewRotation.Equals(TargetRotation, 1.f))
		{
			bIsMoving = false;
			bIsOpen = bNextStateIsOpen;
		}
	}
}

void ADoorGimmick::OnGimmickDestroy()
{
	UE_LOG(LogTemp, Warning, TEXT("DoorGimmick is destroyed"));

	Super::OnGimmickDestroy();
}

void ADoorGimmick::OnGimmickInteracted()
{
	Super::OnGimmickInteracted();

	if (!bIsMoving)
	{
		bNextStateIsOpen = !bIsOpen;
	}
	else
	{
		bNextStateIsOpen = !bNextStateIsOpen;
	}

	bIsMoving = true;

	TargetRotation = bNextStateIsOpen ? OpenRotation : ClosedRotation;
	TargetRotation.Normalize();

	UpdateInteractionText();
}

void ADoorGimmick::UpdateInteractionText() const
{
	const bool bNextWillOpen = bIsMoving ? !bNextStateIsOpen : !bIsOpen;
	InteractableComponent->SetInteractionText(bNextWillOpen ? InteractionTextOpen : InteractionTextClose);
}
