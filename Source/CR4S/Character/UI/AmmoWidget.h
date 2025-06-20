// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

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


	
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> LeftArmCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> RightArmCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> LeftShoulderCurrentAmmo;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,meta=(BindWidgetOptional))
	TObjectPtr<URoundProgressBar> RightShoulderCurrentAmmo;
};
