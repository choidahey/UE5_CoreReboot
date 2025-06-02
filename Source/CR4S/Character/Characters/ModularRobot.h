// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "GameFramework/Character.h"
#include "ModularRobot.generated.h"

class UModularRobotStatusComponent;
class APlayerCharacter;
class UInteractableComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class CR4S_API AModularRobot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AModularRobot();

#pragma region Get
	FORCEINLINE float GetAttackPowerMultiplier() const { return Status->GetAttackPowerMultiplier(); }
#pragma endregion
	
#pragma region ChangePossess
	UFUNCTION(BlueprintCallable)
	void MountRobot(AActor* InActor);
	UFUNCTION(BlueprintCallable)
	void UnMountRobot();
#pragma endregion

#pragma region Widgets
	void InitializeWidgets();
	void DisconnectWidgets();
#pragma endregion
	
#pragma region OverrideFunctions
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
#pragma endregion

#pragma region MoveFunctions
protected:
	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);
#pragma endregion

#pragma region InputActions
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AttackAction;
#pragma endregion

#pragma region MountOffset
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mount", meta = (AllowPrivateAccess = "true"))
	FVector UnMountLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	FName MountSocketName;
#pragma endregion
	
#pragma region Components
private:
	// === Components ===
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UModularRobotStatusComponent> Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractableComponent> InteractComp;
#pragma endregion

#pragma region Cached
	TObjectPtr<APlayerCharacter> MountedCharacter; 
#pragma endregion
};
