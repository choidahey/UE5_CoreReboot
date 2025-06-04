#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ItemRecipeData.generated.h"

USTRUCT(BlueprintType)
struct FRecipeIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작 재료 아이템 행 이름"))
	FName IngredientItemName = NAME_None;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작 재료 필요 개수"))
	int32 Count = 0;
};

USTRUCT(BlueprintType)
struct FItemRecipeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작법 태그"))
	FGameplayTag RecipeTag = FGameplayTag();
	UPROPERTY(EditAnywhere, meta = (DisplayName = "결과 아이템 행 이름"))
	FName ResultItemName = NAME_None;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "결과 아이템 개수", ClampMin = "0"))
	int32 ResultCount = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작 난이도", ClampMin = "0"))
	int32 CraftingDifficulty = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작 재료 목록"))
	TArray<FRecipeIngredient> Ingredients;
};
