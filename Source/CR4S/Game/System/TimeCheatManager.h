#pragma once

#include "GameFramework/CheatManager.h"
#include "TimeCheatManager.generated.h"

UCLASS()
class CR4S_API UTimeCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
    UFUNCTION(exec)
    void AddMinute(int32 Amount);

    UFUNCTION(exec)
    void AddDay(int32 Amount);
};
