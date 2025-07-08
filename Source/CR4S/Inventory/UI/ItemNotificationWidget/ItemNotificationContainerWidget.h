#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemNotificationContainerWidget.generated.h"

class UVerticalBox;
struct FAddItemData;
class UItemNotificationWidget;

UCLASS()
class CR4S_API UItemNotificationContainerWidget : public UUserWidget
{
	GENERATED_BODY()

#pragma region AddItem

public:
	void AddItemNotification(const FAddItemData& AddItemData);

#pragma endregion

#pragma region BindWidget

public:
	FORCEINLINE UVerticalBox* GetNotificationContainer() const { return NotificationContainer; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> NotificationContainer;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemNotificationWidget> NotificationWidgetClass;

	UPROPERTY()
	TArray<TObjectPtr<UItemNotificationWidget>> ActiveNotifications;


#pragma endregion

#pragma region Remove

public:
	UFUNCTION()
	void RemoveNotification(UItemNotificationWidget* NotificationWidget);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxNotifications = 5;

private:
	void RemoveOldestNotification();

#pragma endregion
};
