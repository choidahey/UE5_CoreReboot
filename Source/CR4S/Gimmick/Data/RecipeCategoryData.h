#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "RecipeCategoryData.generated.h"

USTRUCT(BlueprintType)
struct FRecipeCategoryData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "이름"))
	FText Name = FText::FromString("");
	UPROPERTY(EditAnywhere, meta = (DisplayName = "아이콘"))
	TObjectPtr<UTexture2D> Icon = nullptr;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "제작법 태그"))
	FGameplayTag RecipeTag = FGameplayTag();
};
