#include "BaseGimmick.h"

#include "CR4S.h"
#include "Gimmick/Data/GimmickData.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/Components/BaseInventoryComponent.h"

ABaseGimmick::ABaseGimmick()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	GimmickMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	GimmickMeshComponent->SetupAttachment(SceneComponent);
}

void ABaseGimmick::BeginPlay()
{
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	if (CR4S_VALIDATE(LogGimmick, IsValid(World)))
	{
		ItemGimmickSubsystem = World->GetSubsystem<UItemGimmickSubsystem>();
	}
}

const FGimmickInfoData* ABaseGimmick::GetGimmickInfoData() const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
	{
		return nullptr;
	}

	return ItemGimmickSubsystem->FindGimmickInfoData(GimmickDataRowName);
}

void ABaseGimmick::GetResources(const AActor* InventoryOwnerActor) const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventoryOwnerActor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TMap<FName, int32> Resources;
	
	if (const FGimmickInfoData* GimmickData = ItemGimmickSubsystem->FindGimmickInfoData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, MinCount, MaxCount] : GimmickData->Resources)
		{
			const int32 RandomCount = FMath::RandRange(MinCount, MaxCount);

			Resources.FindOrAdd(RowName, RandomCount);
		}
	}

	UBaseInventoryComponent* InventorySystem
		= InventoryOwnerActor->FindComponentByClass<UBaseInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventorySystem)))
	{
		ItemGimmickSubsystem->SpawnItemPouch(this, Resources);
		
		return;
	}

	InventorySystem->AddItems(Resources);
}

void ABaseGimmick::GimmickDestroy()
{
	// CR4S_Log(LogGimmick, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}
