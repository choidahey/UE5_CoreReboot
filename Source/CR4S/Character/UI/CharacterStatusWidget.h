// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

class UBaseStatusComponent;
class UProgressBarWidget;

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
	void UpdateResource(const float InPercentage);

	void InitializeWidget(UBaseStatusComponent* InStatus);

	UFUNCTION(BlueprintCallable)
	void ToggleWidgetMode(const bool bIsRobot) const;
	
	void ClearBindings() const;
#pragma endregion
	
#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> HP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> Resource;
#pragma endregion

#pragma region Cached
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBaseStatusComponent> CachedStatusComponent;
#pragma	endregion
};

