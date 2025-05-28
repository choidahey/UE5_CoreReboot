// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnimalInteractWidget.generated.h"

/**
 * 
 */
UCLASS()
class CR4S_API UAnimalInteractWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitByAnimalState(bool bIsStunned);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CaptureButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ButcherButton;

	UFUNCTION()
	void OnClickedCapture();

	UFUNCTION()
	void OnClickedButcher();

public:
	UPROPERTY()
	TObjectPtr<class ABaseAnimal> OwningAnimal;
};