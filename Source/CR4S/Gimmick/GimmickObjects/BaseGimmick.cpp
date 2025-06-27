#include "BaseGimmick.h"

#include "CR4S.h"
#include "FriendlyAI/Component/ObjectPoolComponent.h"
#include "Game/SaveGame/GimmickSaveGame.h"
#include "Game/System/AudioManager.h"
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

	ObjectPoolComponent = CreateDefaultSubobject<UObjectPoolComponent>(TEXT("ObjectPoolComponent"));
	ObjectPoolComponent->InitialPoolSize = 2;
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

	return ItemGimmickSubsystem->FindGimmickInfoData(GimmickData.RowName);
}

void ABaseGimmick::GetResources(const AActor* InventoryOwnerActor) const
{
	if (!CR4S_VALIDATE(LogGimmick, IsValid(InventoryOwnerActor)) ||
		!CR4S_VALIDATE(LogGimmick, IsValid(ItemGimmickSubsystem)))
	{
		return;
	}

	TMap<FName, int32> Resources;

	if (const FGimmickInfoData* GimmickInfoData = ItemGimmickSubsystem->FindGimmickInfoData(GetGimmickDataRowName()))
	{
		for (const auto& [RowName, MinCount, MaxCount] : GimmickInfoData->Resources)
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

void ABaseGimmick::PlaySFX(USoundBase* SFX, const FVector& Location, const EConcurrencyType SoundType,
                             const float Pitch,
                             const float StartTime) const
{
	const UGameInstance* GameInstance = GetGameInstance();
	if (!CR4S_VALIDATE(LogGimmick, IsValid(GameInstance)))
	{
		return;
	}

	UAudioManager* AudioManager = GameInstance->GetSubsystem<UAudioManager>();
	if (IsValid(AudioManager))
	{
		AudioManager->PlaySFX(SFX, Location, SoundType, Pitch, StartTime);
	}
}

FGimmickSaveGameData ABaseGimmick::GetGimmickSaveGameData_Implementation(bool& bSuccess)
{
	FGimmickSaveGameData GimmickSaveGameData;
	GimmickSaveGameData.GimmickClass = GetClass();
	GimmickSaveGameData.Transform = GetTransform();

	UBaseInventoryComponent* InventoryComponent = FindComponentByClass<UBaseInventoryComponent>();
	if (IsValid(InventoryComponent))
	{
		GimmickSaveGameData.InventorySaveGame = InventoryComponent->GetInventorySaveGame();
	}

	bSuccess = true;

	return GimmickSaveGameData;
}

void ABaseGimmick::LoadGimmickSaveGameData_Implementation(const FGimmickSaveGameData& GimmickSaveGameData)
{
	SetActorTransform(GimmickSaveGameData.Transform);

	UBaseInventoryComponent* InventoryComponent = FindComponentByClass<UBaseInventoryComponent>();
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->LoadInventorySaveGame(GimmickSaveGameData.InventorySaveGame);
	}
}

void ABaseGimmick::GimmickDestroy()
{
	if (IsValid(ObjectPoolComponent))
	{
		// CR4S_Log(LogGimmick, Warning, TEXT("Gimmick return to pool"));
		ObjectPoolComponent->HandleReturnToPool();
	}
	else
	{
		// CR4S_Log(LogGimmick, Warning, TEXT("Gimmick is destroyed"));
		Destroy();
	}
}
