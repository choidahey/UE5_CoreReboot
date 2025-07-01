#pragma once

#include "Blueprint/UserWidget.h"
#include "Game/SaveGame/C4MetaSaveGame.h"
#include "SaveSlotWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;

DECLARE_DELEGATE_OneParam(FOnSlotClicked, int32);

UCLASS()
class CR4S_API USaveSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateSlotInfo(FSaveSlotMetaData SlotData);
	void PlayCreateAnimation();

	void SetSlotThumbnail(const FString& Season);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SlotNameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SaveTimeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayTimeText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> YearText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SeasonText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SlotButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SlotThumbnail;

	UPROPERTY(EditDefaultsOnly, Category = "Thumbnail")
	TObjectPtr <UTexture2D> DefaultImage;
	UPROPERTY(EditDefaultsOnly, Category = "Thumbnail")
	TObjectPtr <UTexture2D> BountifulSeasonImage;
	UPROPERTY(EditDefaultsOnly, Category = "Thumbnail")
	TObjectPtr <UTexture2D> FrostSeasonImage;
	UPROPERTY(EditDefaultsOnly, Category = "Thumbnail")
	TObjectPtr <UTexture2D> RainySeasonImage;
	UPROPERTY(EditDefaultsOnly, Category = "Thumbnail")
	TObjectPtr <UTexture2D> DrySeasonImage;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> OnCreateAnimation;


	UFUNCTION()
	void OnSlotButtonClicked();
public:
	
	FOnSlotClicked OnSlotClicked;

};
