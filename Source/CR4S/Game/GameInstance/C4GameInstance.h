#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "C4GameInstance.generated.h"

UCLASS()
class CR4S_API UC4GameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	FString CurrentSlotName;
};
