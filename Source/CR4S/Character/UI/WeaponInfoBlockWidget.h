// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoBlockWidget.generated.h"

class UWeaponInfoWidget;
class ABaseWeapon;
/**
 * 
 */
UCLASS()
class CR4S_API UWeaponInfoBlockWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

#pragma region Bind & Unbind
	void InitializeWidgetBySlotIndex(ABaseWeapon* InWeapon, const int32 SlotIdx);
	void ClearBindingsToWeapon();
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<UWeaponInfoWidget> LeftArmWeaponInfo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<UWeaponInfoWidget> RightArmWeaponInfo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<UWeaponInfoWidget> LeftShoulderWeaponInfo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<UWeaponInfoWidget> RightShoulderWeaponInfo;

	TArray<TObjectPtr<UWeaponInfoWidget>> WeaponInfoWidgets;
#pragma endregion
};
