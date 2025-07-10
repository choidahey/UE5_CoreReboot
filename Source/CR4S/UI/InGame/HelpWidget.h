#pragma once

#include "Blueprint/UserWidget.h"
#include "Game/Data/HelpMasterDataAsset.h"
#include "HelpWidget.generated.h"

class UCategoryButtonWidget;
class USubCategoryButtonWidget;
class UBaseWindowWidget;
class UScrollBox;
class UHorizontalBox;
class UVerticalBox;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHelpClosed);

UCLASS()
class CR4S_API UHelpWidget : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Constructors & Initiailizers
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

#pragma endregion

#pragma region Widget Classes

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCategoryButtonWidget> CategoryButtonWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USubCategoryButtonWidget> SubCategoryButtonWidgetClass;

#pragma endregion

#pragma region Widget Bindings

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBaseWindowWidget> WindowWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> MainCategoryList;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> SubCategoryList;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> HelpContent;

#pragma endregion

#pragma region Widget Custom
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Button")
	float MainCategorySpacing=10.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom|Button")
	float SubCategorySpacing= 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom|Font")
	TObjectPtr<UFont> HelpFont;

#pragma endregion

#pragma region Internal Logic

protected:
	UFUNCTION()
	void LoadCategory();
	UFUNCTION()
	void LoadSubCategory(const FText& InMainCategoryName);
	UFUNCTION()
	void LoadHelpContent(const FText& InSubCategoryName);

private:
	//void LoadHelpAsset(TFunction<void(UHelpMasterDataAsset*)> Callback);
	UHelpMasterDataAsset* LoadHelpAsset();

	void AddCategory(const FText& InCategoryName);

	void AddSubCategory(const FText& InSubCategoryName);

	void AddImageContent(TSoftObjectPtr<UTexture2D> ImageContent);
	void AddTextContent(const FText& TextContent);

	void ClearMainCategoryList();
	void ClearSubCategoryList();
	void ClearHelpContent();

	void InitHelpWidget();

	UWidget* CreateSpacer(float Width, float Height);

	FText CurrentMainCategory;
	FText CurrentSubCategory;

	TMap<FName, TArray<FHelpContentBlock>> CachedSubCategoryContents;


#pragma endregion

#pragma region Delegates & Events

protected:
	UFUNCTION()
	void OnMainCategoryClicked_Internal(const FText& ClickedCategoryName);

	UFUNCTION()
	void OnSubCategoryClicked_Internal(const FText& ClickedCategoryName);

	UFUNCTION()
	void HandleCloseWindow();

public:
	UPROPERTY(BlueprintAssignable, Category = "Settings")
	FOnHelpClosed OnHelpClosed;
};
