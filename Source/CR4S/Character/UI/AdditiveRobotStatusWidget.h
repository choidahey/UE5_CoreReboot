// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AdditiveRobotStatusWidget.generated.h"

class UProgressBar;
class UModularRobotStatusComponent;
class UBaseStatusComponent;
/**
 * 
 */
UCLASS()
class CR4S_API UAdditiveRobotStatusWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

#pragma region Update
	UFUNCTION(BlueprintCallable)
	void UpdateEnergy(const float InPercentage);
	UFUNCTION(BlueprintCallable)
	void UpdateStun(const float InPercentage);

	UFUNCTION(BlueprintCallable)
	void ToggleWidgetMode(const bool bIsRobot) const;
	
	void InitializeWidget(UBaseStatusComponent* InStatus);
	void ClearBindings() const;
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Energy;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Stun;
#pragma endregion

#pragma region Cached
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UModularRobotStatusComponent> CachedStatusComponent;
#pragma	endregion
};
