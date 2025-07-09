#pragma once

#include "Blueprint/UserWidget.h"
#include "CategoryButtonWidget.generated.h"

class UButton;
class UTextBlock;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryClicked, const FText&, CategoryName);

UCLASS()
class CR4S_API UCategoryButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	

#pragma region Constructors

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

#pragma endregion

#pragma region Widget Bindings

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CategoryButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CategoryText;

	UFUNCTION()
	void OnButtonHovered();
	UFUNCTION()
	void OnButtonUnHovered();
	UFUNCTION()
	void OnButtonClicked();

#pragma endregion

#pragma region Button Custom

public:
	//Call this Function to Dynamically set Button Text//
	UFUNCTION(BlueprintCallable, Category = "Custom|Button")
	void SetCategoryLabel(const FText& InLabel);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Button|Text")
	FText CategoryLabel = FText::FromString(TEXT("Default"));

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

	FVector2D TargetOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Custom|Animation")
	float HoverOffsetAmount = 10.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Animation")
	float LerpSpeed = 10.f;

#pragma endregion


#pragma region Delegates

public:
	//BroadCasted Delegate OnButtonClicked//
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnCategoryClicked OnCategoryClicked;

#pragma endregion

};
