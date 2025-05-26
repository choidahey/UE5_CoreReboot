#include "BaseGimmick.h"

#include "CR4S.h"
#include "Gimmick/Manager/ItemGimmickSubsystem.h"
#include "Inventory/InventoryComponent.h"

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
	if (!IsValid(InventoryOwnerActor))
	{
		CR4S_Log(LogTemp, Warning, TEXT("InventoryOwnerActor is not valid"));
		return;
	}
	
	UInventoryComponent* InventorySystem
		= InventoryOwnerActor->FindComponentByClass<UInventoryComponent>();
	if (!IsValid(InventorySystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("InventorySystem is not valid"));
		return;
	}

	const UItemGimmickSubsystem* ItemGimmickSubsystem = GetGameInstance()->GetSubsystem<UItemGimmickSubsystem>();
	if (!IsValid(ItemGimmickSubsystem))
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemGimmickSubsystem is not valid"));
		return;
	}

	TMap<FName, int32> RemainingItems;
	if (const FBaseGimmickData* GimmickData = ItemGimmickSubsystem->FindGimmickData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, MinCount, MaxCount] : GimmickData->Resources)
		{
			const int32 RandomCount = FMath::RandRange(MinCount, MaxCount);
			
			const FAddItemResult Result
				= InventorySystem->AddItem(RowName, RandomCount);

			UE_LOG(LogTemp, Warning, TEXT("Success: %d / AddCount: %d / RemainingCount: %d")
				   , Result.Success, Result.AddedCount, Result.RemainingCount);
			
			if (Result.RemainingCount > 0)
			{
				RemainingItems[RowName] = Result.RemainingCount;
			}
		}
	}

	if (RemainingItems.Num() > 0)
	{
		// TODO: Spawn Remaining Item Pouch
	}
}

