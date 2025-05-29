#pragma once

#include "UObject/Interface.h"
#include "Spawnable.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*);

UINTERFACE(MinimalAPI)
class USpawnable : public UInterface 
{
	GENERATED_BODY()
};

class CR4S_API ISpawnable
{
	GENERATED_BODY()

public:
	virtual FOnDied* GetOnDiedDelegate() = 0;
};
