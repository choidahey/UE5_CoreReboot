#pragma once

#include "UObject/Interface.h"
#include "Spawnable.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpawnableDestroyed, AActor*);

UINTERFACE(MinimalAPI)
class USpawnable : public UInterface 
{
	GENERATED_BODY()
};

class CR4S_API ISpawnable
{
	GENERATED_BODY()

public:
	virtual FOnSpawnableDestroyed* GetOnSpawnableDestroyedDelegate() = 0;
};
