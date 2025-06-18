#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_HasActorTag.generated.h"

UCLASS()
class CR4S_API UEnvQueryTest_HasActorTag : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_HasActorTag();

protected:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	UPROPERTY(EditAnywhere, Category="EQS")
	FName FilterTag;
};
