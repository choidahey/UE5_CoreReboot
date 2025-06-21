// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockOnWidget.generated.h"

class AHomingWeapon;
class UImage;
/**
 * 
 */
UCLASS()
class CR4S_API ULockOnWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ULockOnWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

#pragma region WidgetUpdate & Bind
	void InitializeWidgetForWeapon(AHomingWeapon* HomingWeapon);
	
	UFUNCTION(BlueprintCallable)
	void UpdateImageVisible();
	UFUNCTION(BlueprintCallable)
	void UpdateImageInvisible();
	UFUNCTION(BlueprintCallable)
	void SetLockedOnColor();
	UFUNCTION(BlueprintCallable)
	void UpdateImagePosition(const FVector2D& NewPosition);
#pragma endregion
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(BindWidgetOptional))
	TObjectPtr<UImage> LockOnImage;
};
