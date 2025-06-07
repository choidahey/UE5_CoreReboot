#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "SpawnZoneVolume.h"
#include "SpawnZoneManager.generated.h"


UCLASS()
class CR4S_API USpawnZoneManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
#pragma region Setters and Getters
public:
	FORCEINLINE int32 SetGridSize(int32 NewGridSize) { return GridSize = NewGridSize; }
	FORCEINLINE int32 GetGridSize() const { return GridSize; }


#pragma region Subsystem Initialization
public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	virtual void Deinitialize() override;
#pragma endregion

#pragma region Zone Management
public:

	void NotifyGridEntered(const FIntPoint& GridCoord);
	void NotifyGridLeft(const FIntPoint& GridCoord);

	void RegisterZone(ASpawnZoneVolume* Zone, const FBox2D& Bounds);

private:
	TMap<FIntPoint, TArray<TWeakObjectPtr<ASpawnZoneVolume>>> GridToZones;
	TMap<TWeakObjectPtr<ASpawnZoneVolume>, int32> ZoneDetectionCounts;

	int32 GridSize = 5000;

#pragma endregion

#pragma region Zone Querying
	

public:
	FIntPoint GetGridCoord(const FVector2D& Location) const;

#if WITH_EDITOR
	void DrawDebugGridCell(const FIntPoint& GridCoord, const FColor& Color, bool Last, float Heigth, float Thickness) const;
#endif

#pragma endregion


};
