// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UInputAction;
UCLASS()
class CR4S_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	ACharacterController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;	

#pragma region IMC
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MenuAction;
	
#pragma endregion

#pragma region Direct Key Binding Handle Function
	UFUNCTION(BlueprintCallable)
	void OnPauseRequested();
#pragma endregion
};
