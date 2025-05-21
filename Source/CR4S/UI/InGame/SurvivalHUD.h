#pragma once

#include "DefaultInGameWidget.h"
#include "TimeDisplayWidget.h"
#include "PauseWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/HUD.h"
#include "Game/System/WorldTimeManager.h"
#include "SurvivalHUD.generated.h"


UCLASS()
class CR4S_API ASurvivalHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UDefaultInGameWidget* GetInGameWidget() const { return InGameWidget; }
#pragma region WidgetSetting
public:
	template<typename TWidget>
	TWidget* CreateAndAddWidget(TSubclassOf<TWidget> WidgetClass, int32 ZOrder, ESlateVisibility InitialVisibility = ESlateVisibility::Collapsed)
	{
		TWidget* Widget = nullptr;
		if (WidgetClass)
		{
			Widget = CreateWidget<TWidget>(GetWorld(), WidgetClass);
			if (Widget)
			{
				Widget->AddToViewport(ZOrder);
				Widget->SetVisibility(InitialVisibility);
			}
		}
		return Widget;
	}
	
	void SetTime(FWorldTimeData CurrentTimeData);
#pragma endregion
#pragma region WidgetClass
protected:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UTimeDisplayWidget> TimeDisplayWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UDefaultInGameWidget> InGameWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UPauseWidget> PauseWidgetClass;
#pragma endregion

#pragma region WidgetInstance
protected:
	//Move to InGameWidget 
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="UI")
	UTimeDisplayWidget* TimeDisplayWidget;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="UI")
	TObjectPtr<UDefaultInGameWidget> InGameWidget;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UPauseWidget> PauseWidget;
#pragma endregion

#pragma region ToggleWidget
public:
	UFUNCTION(BlueprintCallable) //for prototype
	void HandlePauseToggle();

	void ToggleWidget(UUserWidget* Widget);
	void HideWidget(UUserWidget* TargetWidget = nullptr); //Function To Hide Currently Visible Widget Window

	UPROPERTY()
	TArray<UUserWidget*> ActiveWidgets;
#pragma endregion
private:
	virtual void BeginPlay() override;
};
