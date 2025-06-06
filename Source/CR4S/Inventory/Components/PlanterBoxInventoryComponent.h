#pragma once

#include "CoreMinimal.h"
#include "BaseInventoryComponent.h"
#include "PlanterBoxInventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlanterBoxInventoryComponent : public UBaseInventoryComponent
{
	GENERATED_BODY()

#pragma region UBaseInventoryComponent Override

public:
	UPlanterBoxInventoryComponent();

	virtual FAddItemResult AddItem(FName RowName, int32 Count) override;

#pragma endregion

#pragma region Crops

public:
	bool HasCrops() const;

#pragma endregion
};
