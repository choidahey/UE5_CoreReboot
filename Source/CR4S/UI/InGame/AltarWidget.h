#pragma once

#include "Blueprint/UserWidget.h"
#include "AltarWidget.generated.h"

class UButton;
class ADemoAltar;

UCLASS()
class CR4S_API UAltarWidget : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;


#pragma region Widget Bindings

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BountifulSeasonButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FrostSeasonButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RainySeasonButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DrySeasonButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FrostBossButton;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> ShowUIAnim;

#pragma endregion

#pragma region Widget Handlers

	UFUNCTION()
	void OnBountifulSeasonClicked();
	UFUNCTION()
	void OnFrostSeasonClicked();
	UFUNCTION()
	void OnRainySeasonClicked();
	UFUNCTION()
	void OnDrylSeasonClicked();

	UFUNCTION()
	void OnFrostBossClicked();

#pragma endregion

public:
	void BindWidgets();

	void ShowAltarUI();
	void HideAltarUI();

	TObjectPtr<ADemoAltar> Altar = nullptr;
};
