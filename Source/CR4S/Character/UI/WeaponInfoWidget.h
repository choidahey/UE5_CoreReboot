// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoWidget.generated.h"

class ABaseWeapon;
class UTextBlock;
/**
 * 
 */
UCLASS()
class CR4S_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Bind & Unbind
	void InitializeWidgetForWeapon(ABaseWeapon* InWeapon);
	void ClearBindingsToWeapon();
#pragma endregion
	
#pragma region Update
public:
	UFUNCTION(BlueprintCallable)
	void UpdateCurrentAmmoCount(const int32 CurrentAmmoCount);
	UFUNCTION(BlueprintCallable)
	void UpdateMaxAmmoCount(const int32 MaxAmmoCount);

	void SetSlotName(const FText SlotName);
	void StartReload(const float ReloadDuration);
	void StopReload();
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentAmmoCountWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxAmmoCountWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotNameWidget;
#pragma endregion

#pragma region Cached
	UPROPERTY(VisibleAnywhere, Category = "Cached")
	TWeakObjectPtr<ABaseWeapon> CachedWeapon;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Timer")
	FTimerHandle ReloadTimerHandle;
#pragma endregion
	
};
