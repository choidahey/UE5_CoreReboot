#pragma once

#include "Blueprint/UserWidget.h"
#include "PinSettingsWidget.generated.h"

class UButton;
class UEditableText;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAddPinClickedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemovePinClickedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIconClickedEvent, int32, IconIndex);

UCLASS()
class CR4S_API UPinSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FORCEINLINE int32 GetSelectedIconIndex() { return SelectedIconIndex; }
	FORCEINLINE void SetSelectedIconIndex(int32 Index) { SelectedIconIndex = Index; }
	FORCEINLINE int32 GetIconTextureCount() const { return IconTextures.Num(); }

	FSlateBrush GetIconBrush(int32 Index) const;
	FString GetPinName() const;
	void ResetWidgetData();
	void SetWidgetData(int32 Index, FString Name);


	void SetRemoveButtonVisibility(ESlateVisibility InVisibility);

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeDestruct() override;

#pragma region Widget Bindings

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Icon0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Icon1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Icon2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Icon3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Icon4;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> PinNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddPinButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RemovePinButton;
#pragma endregion

#pragma region Delegate Functions

protected:
	UFUNCTION()
	void HandleAddPinClicked();
	UFUNCTION()
	void HandleRemovePinClicked();

	UFUNCTION()
	void HandleIcon0Clicked();
	UFUNCTION()
	void HandleIcon1Clicked();
	UFUNCTION()
	void HandleIcon2Clicked();
	UFUNCTION()
	void HandleIcon3Clicked();
	UFUNCTION()
	void HandleIcon4Clicked();
	UFUNCTION()
	void HandleIconClicked(int32 IconIndex);

#pragma endregion

#pragma region Pin Icons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TArray<UTexture2D*> IconTextures;

	int32 UserIconCount = 0;
#pragma endregion
private:
	int32 SelectedIconIndex = 0;

#pragma region Delegate Declarations 
public:
	FOnAddPinClickedEvent OnAddPinClicked;
	FOnRemovePinClickedEvent OnRemovePinClicked;
	FOnIconClickedEvent OnIconClicked;

#pragma endregion
};
