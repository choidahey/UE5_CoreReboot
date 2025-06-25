#pragma once

#include "UObject/NoExportTypes.h"
#include "SaveGameHelper.generated.h"

UCLASS()
class CR4S_API USaveGameHelper : public UObject
{
	GENERATED_BODY()
	
public:

	void SaveNow();
};
