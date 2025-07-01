#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_BuildingHealthScore.generated.h"

class ABaseBuildingGimmick;

UCLASS()
class CR4S_API UEnvQueryTest_BuildingHealthScore : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_BuildingHealthScore();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};