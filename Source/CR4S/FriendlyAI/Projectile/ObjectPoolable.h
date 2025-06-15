#pragma once

#include "CoreMinimal.h"
#include "ObjectPoolable.generated.h"

UCLASS(Abstract)
class CR4S_API AObjectPoolable : public AActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void OnSpawnFromPool();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnReturnToPool();
};
