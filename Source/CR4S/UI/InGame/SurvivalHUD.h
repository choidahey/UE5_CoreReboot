#pragma once

#include "InGameWidget.h"
#include "TimeDisplayWidget.h"
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
	FORCEINLINE UInGameWidget* GetInGameWidget() const { return InGameWidget; }
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
	TSubclassOf<UInGameWidget> InGameWidgetClass;
#pragma endregion
	
private:
	virtual void BeginPlay() override;
	
#pragma region WidgetInstance
protected:
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="UI")
	UTimeDisplayWidget* TimeDisplayWidget;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly, Category="UI")
	TObjectPtr<UInGameWidget> InGameWidget;
#pragma endregion
	
};
