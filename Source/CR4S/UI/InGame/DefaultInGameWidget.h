#pragma once

#include "Blueprint/UserWidget.h"
#include "Game/System/WorldTimeManager.h"
#include "DefaultInGameWidget.generated.h"

class UEnvironmentalStatusComponent;
class ABaseWeapon;
class UAmmoWidget;
class URoundProgressBar;
class UImage;
class AHomingWeapon;
class ULockOnWidget;
class UBaseStatusComponent;
class UProgressBarWidget;
class UCharacterStatusWidget;
class UPlayerCharacterStatusComponent;
class UCharacterEnvironmentStatusWidget;
class UTimeDisplayWidget;

UCLASS()
class CR4S_API UDefaultInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Get
	FORCEINLINE UCharacterEnvironmentStatusWidget* GetEnvironmentStatusWidget() { return EnvironmentStatusWidget; }
#pragma endregion
	
#pragma region Bind & Unbind
public:
	void ToggleWidgetMode(const bool bIsRobot);
	UFUNCTION(BlueprintCallable)
	void BindLockOnWidgetToHomingWeapon(AHomingWeapon* HomingWeapon);
	UFUNCTION(BlueprintCallable)
	void BindAmmoWidgetToWeapon(ABaseWeapon* InWeapon, const int32 SlotIdx);

	void BindWidgetsToStatus(UBaseStatusComponent* InStatus);
	
	void BindEnvStatusWidgetToEnvStatus(UEnvironmentalStatusComponent* InStatus);

	void ClearBindingsToStatus();
	void ClearBindingsToEnvStatus();
#pragma endregion
	
#pragma region UpdateWidget
public:
	void UpdateHungerWidget(const float InPercentage);
	void UpdateTimeWidget(FWorldTimeData CurrentTimeData);


#pragma endregion

#pragma region CrosshairImage
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair)
	TObjectPtr<UTexture2D> DefaultCrosshair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair)
	TObjectPtr<UTexture2D> RobotCrosshair;
#pragma endregion 
	
#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UCharacterStatusWidget> StatusWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> HungerWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTimeDisplayWidget> TimeDisplayWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UCharacterEnvironmentStatusWidget> EnvironmentStatusWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UImage> CrosshairWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UImage> AimCircle;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<ULockOnWidget> LockOnWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UAmmoWidget> CurrentAmmoWidgets;
#pragma endregion
};
