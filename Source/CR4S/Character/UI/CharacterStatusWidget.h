// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

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
	
	UFUNCTION(BlueprintCallable)
	void UpdateEnergy(const float InPercentage);


	UFUNCTION(BlueprintCallable)
	void UpdateStun(const float InPercentage);

	UFUNCTION(BlueprintCallable)
	void ToggleWidgetMode(UBaseStatusComponent* InComponent, const bool bIsRobot);
#pragma endregion
	
#pragma region Widgets
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> HP;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> Resource;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> Energy;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(BindWidgetOptional))
	TObjectPtr<UProgressBarWidget> Stun;
#pragma endregion
};

