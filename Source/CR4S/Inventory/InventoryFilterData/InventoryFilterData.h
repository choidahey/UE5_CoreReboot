#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "InventoryFilterData.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType)
class CR4S_API UInventoryFilterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	bool IsAllowedItem(const FGameplayTagContainer& ItemTags) const;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Filter")
	FGameplayTagQuery AllowedTagQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Filter")
	FGameplayTagQuery DeniedTagQuery;
};
