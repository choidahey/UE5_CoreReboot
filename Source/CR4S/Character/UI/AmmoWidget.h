// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class ABaseWeapon;
class URoundProgressBar;
/**
 * 
 */
UCLASS()
class CR4S_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UAmmoWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	
#pragma region InitializeWidget
	void InitializeWidgetForWeapon(ABaseWeapon* InWeapon, const int32 SlotIdx);
#pragma endregion

#pragma region UpdateWidget
	void UpdateCurrentAmmoWidget(const int32 SlotIdx, const float Percent);
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> LeftArmCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> RightArmCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> LeftShoulderCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> RightShoulderCurrentAmmo;

	TArray<TObjectPtr<URoundProgressBar>> AmmoProgressWidgets;
#pragma endregion

#pragma region Settings
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Setting")
	float MaxProgress{0.2};
#pragma endregion
};
