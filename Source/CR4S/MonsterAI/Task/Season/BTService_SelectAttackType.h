#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SelectAttackType.generated.h"

enum class ESkillAttackType : uint8;

USTRUCT(BlueprintType)
struct FSkillTypeWeightConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0", ClampMax = "100"))
	float BaseMeleeWeight = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weights", meta = (ClampMin = "0", ClampMax = "100"))
	float BaseRangedWeight = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Thresholds", meta = (ClampMin = "0"))
	float MeleeOptimalDistance = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Thresholds", meta = (ClampMin = "0"))
	float RangedOptimalDistance = 3000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance Influence", meta = (ClampMin = "0", ClampMax = "1"))
	float DistanceInfluenceStrength = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consecutive Prevention")
	bool bPreventConsecutive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consecutive Prevention", meta = (ClampMin = "1", ClampMax = "5"))
	int32 MaxConsecutiveCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consecutive Prevention", meta = (ClampMin = "0", ClampMax = "1"))
	float ConsecutivePenalty = 0.3f;
};

UCLASS()
class CR4S_API UBTService_SelectAttackType : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SelectAttackType();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector AttackTypeKey;
	
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector CurrentDistanceKey;

	UPROPERTY(EditAnywhere, Category = "Skill Selection")
	FBlackboardKeySelector MeleeOptimalDistanceKey;

	UPROPERTY(EditAnywhere, Category = "Skill Selection")
	FBlackboardKeySelector RangedOptimalDistanceKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CurrentPhaseKey;
	
	UPROPERTY(EditAnywhere, Category = "Skill Selection")
	FSkillTypeWeightConfig SkillTypeWeightConfig;
	
	UPROPERTY(EditAnywhere, Category = "Skill Selection")
	TMap<int32, FSkillTypeWeightConfig> PhaseConfigs;

	UPROPERTY(EditAnywhere, Category="Config", meta=(ClampMin="0.0", ClampMax="1.0"))
	float BaseRate = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugInfo = false;

private:
	TMap<TObjectPtr<UBehaviorTreeComponent>, TArray<ESkillAttackType>> RecentSkillHistory;

	ESkillAttackType SelectSkillType(UBehaviorTreeComponent& OwnerComp, float Distance);
	const FSkillTypeWeightConfig& GetActiveConfig(const UBlackboardComponent* BB) const;

	void ComputeRawDistanceWeights(float Distance, const FSkillTypeWeightConfig& Cfg, float& OutMelee, float& OutRanged) const;
	void EnforceConsecutiveRule(UBehaviorTreeComponent& OwnerComp, const FSkillTypeWeightConfig& Cfg, float& InOutMelee, float& InOutRanged);
	void RecordSkillUsage(UBehaviorTreeComponent& OwnerComp, ESkillAttackType Used);
	void NormalizeWeights(float& InOutMelee, float& InOutRanged) const;
};
