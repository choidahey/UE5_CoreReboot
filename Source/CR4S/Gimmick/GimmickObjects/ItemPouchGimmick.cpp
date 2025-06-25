#include "ItemPouchGimmick.h"

#include "CR4S.h"
#include "Game/System/WorldTimeManager.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/OpenWidgetType.h"

AItemPouchGimmick::AItemPouchGimmick()
	: ForwardImpulseStrength(100.f),
	  UpImpulseStrength(50.f),
	  DestroyDelay(180.f),
	  ElapsedSeconds(0.f),
	  PrevPlayTime(-1)
{
	PrimaryActorTick.bCanEverTick = false;

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetMaxInventorySize(100);
	InventoryComponent->SetInventoryTitleText(FText::FromString("ITEM POUCH"));

	GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GimmickMeshComponent->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
}

void AItemPouchGimmick::BeginPlay()
{
	Super::BeginPlay();

	if (CR4S_VALIDATE(LogGimmick, IsValid(InteractableComponent)))
	{
		InteractableComponent->OnTryInteract.AddUniqueDynamic(this, &ThisClass::OnGimmickInteracted);
	}

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		UWorldTimeManager* TimeManager = World->GetSubsystem<UWorldTimeManager>();
		if (IsValid(TimeManager))
		{
			TimeManager->OnWorldTimeUpdated.AddUniqueDynamic(this, &ThisClass::UpdateWorldTime);
		}
	}
}

void AItemPouchGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)) ||
		bIsOpenWidget)
	{
		return;
	}

	if (IsValid(Interactor))
	{
		PlayerInventoryComponent = Interactor->FindComponentByClass<UPlayerInventoryComponent>();
	}

	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	if (IsValid(PlayerInventoryComponent))
	{
		InventoryComponent->OnOccupiedSlotsChange.AddUniqueDynamic(this, &ThisClass::DestroyEmptyItemPouch);

		InventoryComponent->SetMaxInventorySize(InventoryComponent->GetUseSlotCount());

		PlayerInventoryComponent->OpenOtherInventoryWidget(EOpenWidgetType::ItemPouch, InventoryComponent);
		PlayerInventoryComponent->OnInventoryClose.AddUniqueDynamic(this, &ThisClass::UnBoundDelegate);

		bIsOpenWidget = true;
	}
}

void AItemPouchGimmick::InitItemPouch(const AActor* SourceActor, const TMap<FName, int32>& RemainingItems) const
{
	if (CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		InventoryComponent->AddItems(RemainingItems);
	}

	LaunchItemPouch(SourceActor);
}

void AItemPouchGimmick::LaunchItemPouch(const AActor* SourceActor) const
{
	if (IsValid(SourceActor))
	{
		GimmickMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GimmickMeshComponent->SetSimulatePhysics(true);

		FVector ImpulseVector = SourceActor->GetActorForwardVector() * ForwardImpulseStrength;
		ImpulseVector += FVector::UpVector * UpImpulseStrength;

		GimmickMeshComponent->AddImpulse(ImpulseVector, NAME_None, true);
	}
}

void AItemPouchGimmick::UpdateWorldTime(const int64 NewPlayTime)
{
	if (PrevPlayTime < 0)
	{
		PrevPlayTime = NewPlayTime;
		return;
	}

	const int64 DeltaInt = NewPlayTime - PrevPlayTime;
	PrevPlayTime = NewPlayTime;

	if (DeltaInt <= 0)
	{
		return;
	}

	const float DeltaSeconds = static_cast<float>(DeltaInt);
	ElapsedSeconds += DeltaSeconds;

	if (ElapsedSeconds >= DestroyDelay)
	{
		CloseWidget();
		Destroy();
	}
}

void AItemPouchGimmick::DestroyEmptyItemPouch(const int32 NumOccupiedSlots)
{
	if (NumOccupiedSlots == 0)
	{
		PlayerInventoryComponent->CloseInventoryWidget();

		Destroy();
	}
}

void AItemPouchGimmick::UnBoundDelegate()
{
	if (IsValid(PlayerInventoryComponent) &&
		PlayerInventoryComponent->OnInventoryClose.IsAlreadyBound(this, &ThisClass::UnBoundDelegate))
	{
		PlayerInventoryComponent->OnInventoryClose.RemoveDynamic(this, &ThisClass::UnBoundDelegate);
		PlayerInventoryComponent = nullptr;
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AItemPouchGimmick::CloseWidget()
{
	if (bIsOpenWidget && IsValid(PlayerInventoryComponent))
	{
		PlayerInventoryComponent->CloseInventoryWidget();
	}

	bIsOpenWidget = false;
}
