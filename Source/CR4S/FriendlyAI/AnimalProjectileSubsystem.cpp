#include "AnimalProjectileSubsystem.h"
#include "Engine/DataTable.h"
#include "Projectile/Manager/AnimalProjectilePoolManager.h"
#include "Projectile/AnimalProjectile.h"

void UAnimalProjectileSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	static const FString TablePath = TEXT("/Game/CR4S/_Data/Animal/AnimalProjectileDataTable.AnimalProjectileDataTable");
	ProjectileDataTable = LoadObject<UDataTable>(nullptr, *TablePath);
	if (!ProjectileDataTable) return;
	
	for (auto& Pair : ProjectileDataTable->GetRowMap())
	{
		const FName& RowName = Pair.Key;
		const FAnimalProjectileStatsRow* Row = ProjectileDataTable->FindRow<FAnimalProjectileStatsRow>(Pair.Key, TEXT("Initialize"));
		if (!Row) continue;

		FAnimalProjectileInfo Info;
		Info.ProjectileClass = Row->ProjectileClass;
		Info.Speed           = Row->Speed;
		Info.Lifetime        = Row->Lifetime;

		ProjectileInfoMap.Add(RowName, Info);
	}
}

const FAnimalProjectileInfo* UAnimalProjectileSubsystem::GetProjectileInfo(FName RowName) const
{
	return ProjectileInfoMap.Find(RowName);
}
