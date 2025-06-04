#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../AnimalProjectileSubsystem.h"
#include "AnimalRangedAttackComponent.generated.h"

class UAnimalProjectileSubsystem;
class AAnimalProjectile;
class USceneComponent;
class UAnimalProjectilePoolManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAnimalRangedAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAnimalRangedAttackComponent();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category="Ranged Attack", meta=(RowType="AnimalProjectileStatsRow", HideDataTable))
	FDataTableRowHandle ProjectileRowHandle;

private:
	UAnimalProjectileSubsystem* ProjectileSubsystem;
	UAnimalProjectilePoolManager* PoolManager;
	
public:
	UFUNCTION(BlueprintCallable, Category="Ranged Attack")
	void FireProjectile();	
};