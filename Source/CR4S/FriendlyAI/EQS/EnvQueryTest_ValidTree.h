#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_ValidTree.generated.h"

UCLASS()
class CR4S_API UEnvQueryTest_ValidTree : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_ValidTree();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
