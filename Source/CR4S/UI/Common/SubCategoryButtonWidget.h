#pragma once

#include "Blueprint/UserWidget.h"
#include "SubCategoryButtonWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubCategoryClicked, const FText&, CategoryName);

UCLASS()
class CR4S_API USubCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Constructors

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

#pragma endregion

#pragma region Widget Bindings

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SubCategoryButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SubCategoryText;

	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonClicked();

#pragma endregion

#pragma region Button Custom

public:
	//Call this Function to Dynamically set Button Text//
	UFUNCTION(BlueprintCallable, Category = "Custom|Button")
	void SetSubCategoryLabel(const FText& InLabel);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Text")
	FText SubCategoryLabel = FText::FromString(TEXT("Default"));

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Style")
	TObjectPtr<UTexture2D> ButtonTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Sound")
	TObjectPtr<USoundBase> ClickSound;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Sound")
	TObjectPtr<USoundBase> HoverSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Style")
	FLinearColor NormalTint = FLinearColor::Gray;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Style")
	FLinearColor HoveredTint = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Style")
	FLinearColor PressedTint = FLinearColor::Black;

#pragma endregion

#pragma region Delegates

public:
	//BroadCasted Delegate OnButtonClicked//
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnSubCategoryClicked OnSubCategoryClicked;
#pragma endregion
};
