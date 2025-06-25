#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

class UTextBlock;

UCLASS()
class CR4S_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void ShowNotification(const FText& Text, float Duration);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAutoDismiss();

	UPROPERTY(meta= (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeIn;
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOut;

	FTimerHandle AutoDismissHandle;
	FTimerHandle RemoveHandle;

private:
	void SetNotificationText(const FText& Text);
};
