#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class CR4S_API APatrolRoute : public AActor
{
	GENERATED_BODY()
	
public:	
	APatrolRoute();

protected:
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Patrol")
	FVector GetPointAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Patrol")
	int32 GetNumberOfPoints() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USplineComponent> PatrolSpline;



};
