#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQSTest_Pooled.generated.h"

UCLASS()
class CR4S_API UEQSTest_Pooled : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEQSTest_Pooled();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};