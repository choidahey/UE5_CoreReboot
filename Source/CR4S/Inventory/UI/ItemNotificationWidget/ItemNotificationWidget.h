#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNotificationWidget.generated.h"

class ASurvivalHUD;
class UTextBlock;
class UImage;

USTRUCT(BlueprintType)
struct FAddItemData
{
	GENERATED_BODY()

	FAddItemData()
		: ItemIcon(nullptr),
		  ItemCount(0)
	{
	}

	FAddItemData(UTexture2D* NewItemIcon,
	             const int32 NewItemCount,
	             const FText& NewItemName)
		: ItemIcon(NewItemIcon),
		  ItemCount(NewItemCount),
		  ItemName(NewItemName)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ItemCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;
};

UCLASS()
class CR4S_API UItemNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region UUserWidget Overrides

public:
	virtual void NativeConstruct() override;

#pragma endregion

#pragma region AddItem

public:
	UFUNCTION(BlueprintCallable)
	void SetAddItemData(const FAddItemData& AddItemData);

	UFUNCTION(BlueprintCallable)
	void PlaySlideInAnim();
	
	UFUNCTION(BlueprintCallable)
	void StartFadeOut();

#pragma endregion

#pragma region BindWidget

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCount;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> SlideInAnim;

#pragma endregion

#pragma region Remove

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoRemoveTime = 5.0f;

private:
	UFUNCTION()
	void OnFadeOutComplete();

	FTimerHandle AutoRemoveTimer;

#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationRemoved, class UItemNotificationWidget*,
	                                            NotificationWidget);

	UPROPERTY(BlueprintAssignable)
	FOnNotificationRemoved OnNotificationRemoved;

#pragma endregion
};
