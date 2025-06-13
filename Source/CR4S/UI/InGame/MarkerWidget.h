#pragma once

#include "Blueprint/UserWidget.h"
#include "MarkerWidget.generated.h"

class UImage;
class UPinSettingsWidget;
class USoundBase;

DECLARE_DELEGATE_OneParam(FOnPinSelected, UMarkerWidget*);

USTRUCT(BlueprintType)
struct FPinData
{
	GENERATED_BODY()

	int32 IconIndex = 0;
	FString PinName = TEXT("DefaultName");
	FVector2D Position = FVector2D::ZeroVector;
};

UCLASS()
class CR4S_API UMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	FORCEINLINE void SetPinData(const FPinData& Data) { PinData = Data; }
	FORCEINLINE void SetParentRef(UPinSettingsWidget* Ref) { PinSettingsRef = Ref; }

	FORCEINLINE FPinData GetPinData() const { return PinData; }

	void SetPinImage(FSlateBrush& Brush);
	void SetSelected();
	void SetDeSelected();
	void RemovePin();

	bool bAllowEdit = true;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual void NativeDestruct() override;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PinImage;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Hover;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* UnHover;


protected:
	UPROPERTY()
	TObjectPtr<UPinSettingsWidget> PinSettingsRef = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundBase* ClickSound;

private:
	FPinData PinData;

	bool bIsHovered = false;
	bool bIsSelected = false;

public:
	FOnPinSelected OnPinSelected;

};
