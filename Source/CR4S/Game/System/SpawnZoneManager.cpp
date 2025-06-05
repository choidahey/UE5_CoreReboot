#include "Game/System/SpawnZoneManager.h"
#include "Game/System/SpawnZoneVolume.h"
#include "Game/System/EnvironmentManager.h"
#include "EngineUtils.h"    
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

bool USpawnZoneManager::ShouldCreateSubsystem(UObject* Outer) const
{
	UWorld* World = Cast<UWorld>(Outer);
	if (World && World->GetName() == TEXT("SurvivalLevel"))
	{
		return true;  // Creates this subsystem only in the SurvivalLevel world
	}
	return false;
}

void USpawnZoneManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &USpawnZoneManager::OnPostWorldInit);
}

void USpawnZoneManager::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
	if (!World || World->WorldType != EWorldType::Game && World->WorldType != EWorldType::PIE)
	{
		return;
	}

    AEnvironmentManager* EnvManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(World, AEnvironmentManager::StaticClass()));
    if (EnvManager)
    {
        GridSize = EnvManager->GetGridSize();
    }
}

void USpawnZoneManager::Deinitialize()
{
	Super::Deinitialize();
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
}

void USpawnZoneManager::RegisterZone(ASpawnZoneVolume* Zone, const FBox2D& Bounds)
{
	if (!Zone || !Bounds.bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Zone or Bounds in RegisterZone"));
		return;
	}

    FIntPoint Min = GetGridCoord(Bounds.Min);
    FIntPoint Max = GetGridCoord(Bounds.Max);

    for (int32 X = Min.X; X <= Max.X; ++X)
    {
        for (int32 Y = Min.Y; Y <= Max.Y; ++Y)
        {
            FIntPoint Coord(X, Y);
            GridToZones.FindOrAdd(Coord).Add(Zone);
            DrawDebugGridCell(Coord, FColor::Green, true, 10.0f, 5.0f);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Zone registered from (%d, %d) to (%d, %d)"), Min.X, Min.Y, Max.X, Max.Y);
}

FIntPoint USpawnZoneManager::GetGridCoord(const FVector2D& Location) const
{
    return FIntPoint(
        FMath::FloorToInt(Location.X / GridSize),
        FMath::FloorToInt(Location.Y / GridSize)
    );
}

void USpawnZoneManager::NotifyGridEntered(const FIntPoint& GridCoord)
{
    auto* Zones = GridToZones.Find(GridCoord);
    if (Zones)
    {
        for (auto& WeakZone : *Zones)
        {
            if (ASpawnZoneVolume* Zone = WeakZone.Get())
            {
                int32& Count = ZoneDetectionCounts.FindOrAdd(Zone);
                if (Count == 0)
                {
                    Zone->SpawnActorsInZone();
                }
                Count++;

				UE_LOG(LogTemp, Log, TEXT("Zone %s entered grid (%d, %d), count: %d"), *Zone->GetName(), GridCoord.X, GridCoord.Y, Count);
            }
        }
    }
}


void USpawnZoneManager::NotifyGridLeft(const FIntPoint& GridCoord)
{
    auto* Zones = GridToZones.Find(GridCoord);
    if (Zones)
    {
        for (auto& WeakZone : *Zones)
        {
            if (ASpawnZoneVolume* Zone = WeakZone.Get())
            {
                int32* CountPtr = ZoneDetectionCounts.Find(Zone);
                if (CountPtr)
                {
                    (*CountPtr)--;
					UE_LOG(LogTemp, Log, TEXT("Zone %s left grid (%d, %d), count: %d"), *Zone->GetName(), GridCoord.X, GridCoord.Y, *CountPtr);
                    if (*CountPtr <= 0)
                    {
                        Zone->DespawnActorsInZone();
                        ZoneDetectionCounts.Remove(Zone);
                    }
                }
            }
        }
    }
}

void USpawnZoneManager::DrawDebugGridCell(const FIntPoint& Coord, const FColor& Color, bool Last, float Height, float Thickness) const
{
    FVector Center = FVector(
        (Coord.X + 0.5f) * GridSize,
        (Coord.Y + 0.5f) * GridSize,
        Height
    );

    FVector Extent = FVector(GridSize * 0.5f, GridSize * 0.5f, 5.0f); // ¾ãÀº ¹Ú½º

    DrawDebugBox(
        GetWorld(),
        Center,
        Extent,
        Color,
        Last,
        0.5f,   
        0,         
        Thickness       
    );
}