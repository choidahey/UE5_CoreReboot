#pragma once

#include "Engine/DataTable.h"
#include "HelperBotStatsRow.generated.h"

USTRUCT(BlueprintType)
struct FHelperBotStatsRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpHeight = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 0.f;
	
};
