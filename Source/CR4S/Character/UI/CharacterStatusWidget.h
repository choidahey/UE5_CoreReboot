// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class CR4S_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCharacterStatusWidget(const FObjectInitializer& ObjectInitializer);

#pragma region Update
	UFUNCTION(BlueprintCallable)
	void UpdateHP(const float InPercentage);

	UFUNCTION(BlueprintCallable)
	void UpdateHunger(const float InPercentage);

	UFUNCTION(BlueprintCallable)
	void UpdateStamina(const float InPercentage);

#pragma endregion
	
#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> HP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Hunger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> Stamina;

#pragma endregion
};
