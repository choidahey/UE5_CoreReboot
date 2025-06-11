#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckNearestHouse.generated.h"

UCLASS()
class CR4S_API UBTService_CheckNearestHouse : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CheckNearestHouse();

protected:
	virtual void TickNode(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;
	
	virtual void OnBecomeRelevant(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NearestHouseActor;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HouseAcceptanceRadiusKey;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard", meta = (ClampMin = "0.0"))
	float RadiusOffset = 100.f;  

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FName HouseTag = TEXT("House");

private:
	AActor* FindNearestHouse(const FVector& PlayerLocation) const;
	float CalculateActorRadius(const AActor* InActor) const;

	UPROPERTY()
	APawn* CachedOwnerPawn = nullptr;
};
