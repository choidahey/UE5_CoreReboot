#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_BuildingNeedsRepair.generated.h"

class UDestructibleComponent;

UCLASS()
class CR4S_API UEnvQueryTest_BuildingNeedsRepair : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_BuildingNeedsRepair();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};