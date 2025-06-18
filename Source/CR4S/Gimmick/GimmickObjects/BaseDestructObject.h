#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseDestructObject.generated.h"

UCLASS()
class CR4S_API ABaseDestructObject : public AActor
{
	GENERATED_BODY()

#pragma region AActor Override
	
public:
	ABaseDestructObject();

	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

#pragma endregion

#pragma region Destroy

private:
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyImpulseRadius;
	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float DestroyImpulseStrength;

	UPROPERTY(EditAnywhere, Category = "Destroy", meta = (ClampMin = 0.0))
	float LifeTime;

#pragma endregion
	
};
