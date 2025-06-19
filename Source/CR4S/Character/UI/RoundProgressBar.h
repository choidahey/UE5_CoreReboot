// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundProgressBar.generated.h"

class AHomingWeapon;
class UImage;
/**
 * 
 */
UCLASS()
class CR4S_API URoundProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	URoundProgressBar(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetPercent(const float InPercent);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UImage> RoundImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> ParentMaterial;
};
