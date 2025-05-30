#include "AnimalProjectilePoolManager.h"
#include "../AnimalProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../../AnimalProjectileSubsystem.h"
#include "Game/GameInstance/C4GameInstance.h"

UAnimalProjectilePoolManager* UAnimalProjectilePoolManager::Get(UWorld* World)
{
    if (!World) return nullptr;

    if (UC4GameInstance* GI = Cast<UC4GameInstance>(World->GetGameInstance()))
    {
        if (!GI->ProjectilePool)
        {
            GI->ProjectilePool = NewObject<UAnimalProjectilePoolManager>(GI);
        }
        return GI->ProjectilePool;
    }
    return nullptr;
}

void UAnimalProjectilePoolManager::InitializePool(int32 PoolSize)
{
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    if (!World) return;

    if (UAnimalProjectileSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<UAnimalProjectileSubsystem>())
    {
        TSet<FString> Initialized;

        for (const auto& Pair : Subsystem->GetProjectileInfoMap())
        {
            const FAnimalProjectileInfo& Info = Pair.Value;
            if (!Info.ProjectileClass) continue;

            const FString ClassPath = Info.ProjectileClass->GetPathName();
            if (Initialized.Contains(ClassPath)) continue;

            TArray<AAnimalProjectile*>& Pool = PoolMap.FindOrAdd(Info.ProjectileClass);

            for (int32 i = 0; i < PoolSize; ++i)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                AAnimalProjectile* Proj = World->SpawnActor<AAnimalProjectile>(Info.ProjectileClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
                Proj->SetActorHiddenInGame(true);
                Proj->SetActorEnableCollision(false);
                Pool.Add(Proj);
            }

            Initialized.Add(ClassPath);
        }
    }
}

AAnimalProjectile* UAnimalProjectilePoolManager::Acquire(TSubclassOf<AAnimalProjectile> ProjectileClass)
{
    TArray<AAnimalProjectile*>* Pool = PoolMap.Find(ProjectileClass);
    if (Pool && Pool->Num() > 0)
    {
        AAnimalProjectile* Proj = (*Pool)[0];
        Pool->RemoveAtSwap(0);

        Proj->SetActorLocation(FVector::ZeroVector);

        if (Proj->ProjectileMovement)
        {
            Proj->ProjectileMovement->Velocity = FVector::ZeroVector;
        }

        Proj->SetActorHiddenInGame(false);
        Proj->SetActorEnableCollision(true);
        return Proj;
    }
    return nullptr;
}

void UAnimalProjectilePoolManager::Release(AAnimalProjectile* Projectile)
{
    if (!Projectile) return;

    Projectile->SetActorEnableCollision(false);
    Projectile->SetActorHiddenInGame(true);
    Projectile->SetActorLocation(FVector::ZeroVector);

    if (Projectile->ProjectileMovement)
    {
        Projectile->ProjectileMovement->StopMovementImmediately();
    }

    TSubclassOf<AAnimalProjectile> Cls = Projectile->GetClass();
    if (TArray<AAnimalProjectile*>* Pool = PoolMap.Find(Cls))
    {
        Pool->Add(Projectile);
    }
}
