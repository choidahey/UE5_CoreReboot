#pragma once

#include "GameFramework/Actor.h"
#include "Game/System/SeasonManager.h"
#include "SpawnZoneVolume.generated.h"

class USceneComponent;
class USplineComponent;
class USpawnZoneManager;

USTRUCT(BlueprintType)
struct FSpawnClassInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    TSubclassOf<AActor> SpawnClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    int32 SpawnCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float RespawnTime = 5.0f;
};


UCLASS()
class CR4S_API ASpawnZoneVolume : public AActor
{
	GENERATED_BODY()
	
#pragma region Life Cycle Functions
public:
    ASpawnZoneVolume();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

#pragma endregion

#if WITH_EDITOR
    virtual void OnConstruction(const FTransform& Transform) override;
#endif

#pragma region Components & Parameters
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;
    UPROPERTY(VisibleAnywhere)
    USplineComponent* SplineComponent;

    UPROPERTY(EditAnywhere, Category = "Spawn|Actor")
    TArray<FSpawnClassInfo> BountifulSeasonSpawns;
    UPROPERTY(EditAnywhere, Category = "Spawn|Actor")
    TArray<FSpawnClassInfo> FrostSeasonSpawns;
    UPROPERTY(EditAnywhere, Category = "Spawn|Actor")
    TArray<FSpawnClassInfo> RainySeasonSpawns;
    UPROPERTY(EditAnywhere, Category = "Spawn|Actor")
    TArray<FSpawnClassInfo> DrySeasonSpawns;

    UPROPERTY(EditAnywhere) //Used in Fallback
    float SpawnHeight = 100.f;

#pragma endregion

#pragma region Utility Functions
public:
    bool GetRandomPointInPolygon(const TArray<FVector2D>& Polygon, FVector2D& OutPoint) const;
    
    FBox2D CalculateSplineBounds2D() const;

    TArray<FVector2D> GetPolygonFromSpline() const;



#if WITH_EDITOR
    void DrawDebugLines();
#endif

#pragma endregion

#pragma region Spawn, Despawn and Respawn

public:
    void SetZoneActive(bool bNewActive);

    void SpawnActorsInZone();
    void DespawnActorsInZone();

protected:
    UFUNCTION()
    void HandleActorDeath(AActor* Actor);
    UFUNCTION()
    void RespawnActor(TSubclassOf<AActor> ActorClass);
    UFUNCTION()
    void OnSeasonChanged(ESeasonType Season);

    void UpdateSeasonSpawns(ESeasonType Season);

    AActor* SpawnActorWithDelegate(TSubclassOf<AActor> SpawnClass, const FVector& Location);
    bool TryGetGroundSpawnLocation(const FVector2D& Point2D, FVector& OutLocation) const;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> SpawnedActors;

	USpawnZoneManager* SpawnZoneManager = nullptr;
    const TArray<FSpawnClassInfo>* SeasonSpawns = nullptr;

private:
    bool bIsZoneActive = false;
    

#pragma endregion
};

#pragma region Polygon Helper

namespace FPolygon2D
{
    // ray casting
    static bool PointInPolygon(const FVector2D& Point, const TArray<FVector2D>& Polygon)
    {
        int32 Intersections = 0;
        int32 Count = Polygon.Num();

        for (int32 i = 0; i < Count; ++i)
        {
            const FVector2D& A = Polygon[i];
            const FVector2D& B = Polygon[(i + 1) % Count];

            if (((A.Y <= Point.Y) && (B.Y > Point.Y)) || ((A.Y > Point.Y) && (B.Y <= Point.Y)))
            {
                float Slope = (B.X - A.X) / (B.Y - A.Y);
                float XAtY = A.X + (Point.Y - A.Y) * Slope;
                if (XAtY > Point.X)
                    ++Intersections;
            }
        }

        return (Intersections % 2) == 1;
    }
}

#pragma endregion