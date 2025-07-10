#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseWindowWidget.generated.h"

class UButtonWidget;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApplyClicked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackClicked);

UCLASS()
class CR4S_API UBaseWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Window")
	FOnApplyClicked OnApplyClicked;
	UPROPERTY(BlueprintAssignable, Category = "Window")
	FOnBackClicked OnBackClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void OnBackButtonClicked();
	UFUNCTION()
	void OnApplyButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButtonWidget* BackButton;
	UPROPERTY(meta = (BindWidget))
	UButtonWidget* ApplyButton;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WindowTitle;
	UPROPERTY(meta = (BindWidget))
	UImage* WindowIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Window")
	FText TitleText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Window")
	UTexture2D* IconTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Window")
	bool bShowButtons = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Window")
	bool bShowLeftButton = true;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Window")
	bool bShowRightButton = true;
};
