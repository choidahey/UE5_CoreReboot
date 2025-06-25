#pragma once

#include "UObject/NoExportTypes.h"
#include "TimeCheatHelper.generated.h"

UCLASS()
class CR4S_API UTimeCheatHelper : public UObject
{
	GENERATED_BODY()
	
public:
	void AddMinute(int32 Amount);
	void AddDay(int32 Amount);

};
