#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "JumpHeightTest.generated.h"

UCLASS()
class CR4S_API UJumpHeightTest : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UJumpHeightTest();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

	UPROPERTY(EditDefaultsOnly, Category = "JumpTest")
	float MaxJumpHeight = 300.f;
};
