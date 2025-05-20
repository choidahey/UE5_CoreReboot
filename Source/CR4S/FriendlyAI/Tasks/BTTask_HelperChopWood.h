#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_HelperChopWood.generated.h"

UCLASS()
class CR4S_API UBTTask_HelperChopWood : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HelperChopWood();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
	) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ResourceTargetKey;
};
