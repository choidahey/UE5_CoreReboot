#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MonsterAI/Data/MonsterEnum.h"
#include "BossPhaseDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FPhaseTransitionCondition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EBossPhase TargetPhase;

	UPROPERTY(EditAnyWhere)
	float HPThresholdRatio = 1.f;

	UPROPERTY(EditAnywhere)
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere)
	float SpeedMultiplier = 1.0f;
};

UCLASS()
class CR4S_API UBossPhaseDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TArray<FPhaseTransitionCondition> PhaseConditions;

};
