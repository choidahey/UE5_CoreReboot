// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DefaultInGameWidget.generated.h"

class UCharacterStatusWidget;
class UPlayerCharacterStatusComponent;
/**
 * 
 */
UCLASS()
class CR4S_API UDefaultInGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

#pragma region Initialize
public:
	void InitializeWidget(UPlayerCharacterStatusComponent* InComponent);
#pragma endregion
	
#pragma region UpdateWidget
public:
	void UpdateHPWidget(const float InPercentage);
	void UpdateHungerWidget(const float InPercentage);
	void UpdateStaminaWidget(const float InPercentage);
#pragma endregion

#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UCharacterStatusWidget> StatusWidget;
#pragma endregion
	
};
