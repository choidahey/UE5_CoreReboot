#pragma once

#include "Blueprint/UserWidget.h"
#include "BaseWindowWidget.generated.h"

class UButton;
class UTextBlock;

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
	FOnApplyClicked OnBackClicked;

	void SetTitleText(FText NewText);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnBackButtonClicked();
	UFUNCTION()
	void OnApplyButtonClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ApplyButton;
	UPROPERTY(Meta = (BindWidget))
	UTextBlock* TitleTextBlock;
};
