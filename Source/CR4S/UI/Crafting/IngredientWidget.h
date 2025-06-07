#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngredientWidget.generated.h"

class UProgressBar;
class UImage;
class UTextBlock;

USTRUCT()
struct FIngredientData
{
	GENERATED_BODY()

	FIngredientData()
	{
	}

	FIngredientData(UTexture2D* NewIcon,
	                const FText& NewName,
	                const int32 NewCurrentCount,
	                const int32 NewRequiredCount)
		: Icon(NewIcon),
		  Name(NewName),
		  CurrentCount(NewCurrentCount),
		  RequiredCount(NewRequiredCount)

	{
	}

	UPROPERTY()
	TObjectPtr<UTexture2D> Icon = nullptr;
	FText Name = FText();
	int32 CurrentCount = 0;
	int32 RequiredCount = 0;
};

UCLASS()
class CR4S_API UIngredientWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Initialize

public:
	void InitWidget(const FIngredientData& IngredientData) const;

#pragma endregion


#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IngredientIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> IngredientName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ResourceProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RequiredIngredientCount;

#pragma endregion

#pragma region ProgressBar

public:
	void UpdateProgressBar(int32 CurrentCount, int32 RequiredCount) const;

private:
	UPROPERTY(EditDefaultsOnly, Category="ProgressColor")
	FLinearColor ColorAtZero = FLinearColor::Red;
	UPROPERTY(EditDefaultsOnly, Category="ProgressColor")
	FLinearColor ColorAtOne = FLinearColor::Green;

#pragma endregion
};
