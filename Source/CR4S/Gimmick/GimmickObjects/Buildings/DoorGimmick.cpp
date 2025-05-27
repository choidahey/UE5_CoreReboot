#include "DoorGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"

ADoorGimmick::ADoorGimmick()
	: InteractionTextOpen(FText::FromString(TEXT("Open")))
	  , InteractionTextClose(FText::FromString(TEXT("Close")))
	  , bIsMoving(false)
	  , bIsOpen(false)
	  , bNextStateIsOpen(false)
	  , OpenAngle(90.f)
	  , DoorSpeed(90.f)
	  , ClosedRotation(FRotator::ZeroRotator)
	  , OpenRotation(FRotator::ZeroRotator)
	  , TargetRotation()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
}

void ADoorGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.BindDynamic(this, &ThisClass::OnGimmickInteracted);
	}

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

void ADoorGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)))
	{
		return;
	}

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
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		return;
	}

	const bool bNextWillOpen = bIsMoving ? !bNextStateIsOpen : !bIsOpen;
	InteractableComponent->SetInteractionText(bNextWillOpen ? InteractionTextOpen : InteractionTextClose);
}
