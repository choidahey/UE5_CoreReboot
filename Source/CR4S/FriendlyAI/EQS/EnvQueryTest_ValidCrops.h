#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_ValidCrops.generated.h"

UCLASS()
class CR4S_API UEnvQueryTest_ValidCrops : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_ValidCrops();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};