// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "PlayerCharacter.generated.h"

class UInteractionComponent;
class UPlayerCharacterStatusComponent;
class UCombatComponent;
struct FInputActionValue;
class UAlsCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS(AutoExpandCategories = ("Settings|Player Character"))
class CR4S_API APlayerCharacter : public AAlsCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

#pragma region Get
	FORCEINLINE UStaticMeshComponent* GetOverlayStaticMesh() { return OverlayStaticMesh; }
	FORCEINLINE int32 GetAttackPower() const { return Status->GetAttackPower(); }
#pragma endregion
	
#pragma region Widget
	void InitializeWidgets();
	void DisconnectWidgets();
#pragma endregion
	
#pragma region Overrides
public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;
	virtual void NotifyControllerChanged() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& ViewInfo) override;
	virtual void SetupPlayerInputComponent(UInputComponent* Input) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
#pragma endregion
	
#pragma region InputFunction
private:
	virtual void Input_OnLookMouse(const FInputActionValue& ActionValue);

	virtual void Input_OnLook(const FInputActionValue& ActionValue);

	virtual void Input_OnMove(const FInputActionValue& ActionValue);

	virtual void Input_OnSprint(const FInputActionValue& ActionValue);

	virtual void Input_OnWalk();

	virtual void Input_OnCrouch();

	virtual void Input_OnJump(const FInputActionValue& ActionValue);

	virtual void Input_OnAim(const FInputActionValue& ActionValue);

	virtual void Input_OnRagdoll();

	virtual void Input_OnRoll();

	virtual void Input_OnRotationMode();

	virtual void Input_OnViewMode();

	virtual void Input_OnSwitchShoulder();
#pragma endregion

#pragma region Components
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UAlsCameraComponent> Camera;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<USkeletalMeshComponent> VisibleMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UCombatComponent> Combat;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UPlayerCharacterStatusComponent> Status;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UInteractionComponent> Interaction;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<USkeletalMeshComponent> OverlaySkeletalMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UStaticMeshComponent> OverlayStaticMesh;
#pragma endregion

#pragma region InputActions
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> WalkAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RagdollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> RotationModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> ViewModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> SwitchShoulderAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> AttackAction;
#pragma endregion
	
#pragma region LookSettings
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Player Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookUpMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Player Character", Meta = (ClampMin = 0, ForceUnits = "x"))
	float LookRightMouseSensitivity{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Player Character", Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookUpRate{90.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Player Character", Meta = (ClampMin = 0, ForceUnits = "deg/s"))
	float LookRightRate{240.0f};
#pragma endregion
	


#pragma region Navigation Invokers

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Component")
	float NavGenerationRadius;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Monster|Component")
	float NavRemovalRadius;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Monster|Component")
	TObjectPtr<class UNavigationInvokerComponent> NavInvoker;

#pragma endregion

};
