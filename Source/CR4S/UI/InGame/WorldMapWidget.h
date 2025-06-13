#pragma once

#include "Blueprint/UserWidget.h"
#include "UI/InGame/MarkerWidget.h"
#include "WorldMapWidget.generated.h"

class UImage;
class UButton;
class UOverlay;
class UMarkerWidget;
class UPinSettingsWidget;

UCLASS()
class CR4S_API UWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region Constructors and Event Handlers

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeDestruct() override;

#pragma region WidgetBindings

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> MapOverlay;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MapImage;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MarkerIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PinSettingsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPinSettingsWidget> PinSettingsWidget;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PullPinSettings;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* PushPinSettings;

#pragma endregion

#pragma region PinSettingsFunctions

protected:
	UFUNCTION()
	void OnPinSettingsButtonClicked();

	UFUNCTION()
	void HandleAddPin();

	UFUNCTION()
	void HandleRemovePin();

	UFUNCTION()
	void HandleIconSelection(int32 IconIndex);
	UFUNCTION()
	void HandlePinSelectionChanged(UMarkerWidget* SelectedPin);

#pragma endregion

#pragma region EditableVariables

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FVector2D WorldMin = FVector2D(-110000.f, -10000.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	FVector2D WorldMax = FVector2D(150000.f, 120000.f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map")
	TSubclassOf<UMarkerWidget> PinMarkerWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Sound")
	USoundBase* OpenMapSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect|Sound")
	USoundBase* CloseMapSound;

#pragma endregion

#pragma region State And Internal Data

protected:

	UPROPERTY()
	FSlateBrush DefaultMarkerBrush;

	bool bIsPinSettingsOpen = false;
	FVector2D CurrentMarkerPos = FVector2D::ZeroVector;
	TObjectPtr<UMarkerWidget> CurrentPin = nullptr;

	TArray<FPinData> SavedPins;


#pragma endregion

#pragma region Internal Functions

public:
	UFUNCTION(BlueprintCallable, Category = "Map")
	UMarkerWidget* AddMarkerByWorldLocation(const FVector& WorldLocation, int32 IconIndex = 0, FText PinName = FText::GetEmpty(), bool bEditable = true);

	UFUNCTION(BlueprintCallable, Category = "Map")
	void DeleteMarker(UMarkerWidget* Marker);


	UMarkerWidget* AddMarkerByMapPosition(const FVector2D& MapPosition, int32 IconIndex = 0, FText PinName = FText::GetEmpty(), bool bEditable = true);

	void UpdateWorldMap();
	void HideMarker();

	void PlayOpenMapSound();
	void PlayCloseMapSound();

private:
	UFUNCTION(BlueprintCallable, Category = "Map")
	FVector2D WorldToMapPosition(const FVector& WorldLocation) const;

	UMarkerWidget* FindMarkerAtPosition(const FVector2D& Position) const;
	bool IsSamePosition(const FVector2D& A, const FVector2D& B, float Tolerance = 0.1f) const;

	UMarkerWidget* CreateAndAddMarker(const FVector2D& MapPosition, int32 IconIndex, const FString& PinName, bool bEditable);

	bool TryRemoveEditableMarkerAtPosition(const FVector2D& Position);

	void TogglePinSettingsPanel();

	void FindPlayerLocation();
	void ResetMarkerIcon();
#pragma endregion
};
