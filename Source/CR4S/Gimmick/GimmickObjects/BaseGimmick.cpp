#include "BaseGimmick.h"

#include "CR4S.h"
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

void ABaseGimmick::GetResources(const AActor* InventoryOwnerActor) const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventoryOwnerActor)))
	{
		return;
	}
	
	UBaseInventoryComponent* InventorySystem
		= InventoryOwnerActor->FindComponentByClass<UBaseInventoryComponent>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventorySystem)))
	{
		return;
	}

	const UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}
	
	if (const FBaseGimmickData* GimmickData = ItemGimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, MinCount, MaxCount] : GimmickData->Resources)
		{
			const int32 RandomCount = FMath::RandRange(MinCount, MaxCount);
			
			const FAddItemResult Result
				= InventorySystem->AddItem(RowName, RandomCount);

			CR4S_Log(LogGimmick, Warning, TEXT("Success: %d / AddCount: %d / RemainingCount: %d")
				   , Result.Success, Result.AddedCount, Result.RemainingCount);
		}
	}
}

void ABaseGimmick::GimmickDestroy()
{
	CR4S_Log(LogGimmick, Warning, TEXT("Gimmick is destroyed"));

	Destroy();
}

