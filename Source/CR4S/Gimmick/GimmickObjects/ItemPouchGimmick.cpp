#include "ItemPouchGimmick.h"

#include "CR4S.h"
#include "Gimmick/Components/InteractableComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "Inventory/OpenWidgetType.h"

AItemPouchGimmick::AItemPouchGimmick()
	: ForwardImpulseStrength(100.f),
	  UpImpulseStrength(50.f)
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

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (IsValid(PlayerController))
	{
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (IsValid(PlayerController->GetPawn()))
		{
			PlayerInventoryComponent = PlayerPawn->FindComponentByClass<UPlayerInventoryComponent>();
		}
	}
}

void AItemPouchGimmick::OnGimmickInteracted(AActor* Interactor)
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(Interactor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(InventoryComponent)))
	{
		return;
	}

	if (!CR4S_VALIDATE(LogGimmick, IsValid(PlayerInventoryComponent)))
	{
		return;
	}

	if (IsValid(PlayerInventoryComponent))
	{
		InventoryComponent->OnOccupiedSlotsChanged.BindDynamic(this, &ThisClass::DestroyItemPouch);

		InventoryComponent->SetMaxInventorySize(InventoryComponent->GetUseSlotCount());

		PlayerInventoryComponent->OpenOtherInventoryWidget(EOpenWidgetType::ItemPouch, InventoryComponent);
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

void AItemPouchGimmick::DestroyItemPouch(const int32 NumOccupiedSlots)
{
	if (NumOccupiedSlots == 0)
	{
		PlayerInventoryComponent->CloseInventoryWidget();

		Destroy();
	}
}
