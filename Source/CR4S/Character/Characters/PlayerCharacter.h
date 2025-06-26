// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AlsCharacter.h"
#include "Game/Interface/SavableActor.h"
#include "Character/Components/PlayerCharacterStatusComponent.h"
#include "Character/Data/PlayerCharacterSettingsDataAsset.h"
#include "Game/SaveGame/CoreSaveGame.h"
#include "PlayerCharacter.generated.h"

class USaveGame;
class UPlayerInventoryComponent;
struct FPlayerCharacterSettings;
class UInputBufferComponent;
class UPlayerInputBufferComponent;
class APlayerTool;
class ABaseTool;
class UWeaponTraceComponent;
class UInteractionComponent;
class UPlayerCharacterStatusComponent;
struct FInputActionValue;
class UAlsCameraComponent;
class UInputMappingContext;
class UInputAction;
class UEnvironmentalStatusComponent;
class UGridDetectionComponent;

UCLASS(AutoExpandCategories = ("Settings|Player Character"))
class CR4S_API APlayerCharacter : public AAlsCharacter, public ISavableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

#pragma region Save/Load
	virtual FName GetUniqueSaveID() override;
	virtual void SetUniqueSaveID(FName NewID) override;
	virtual void GatherSaveData(FSavedActorData& OutSaveData) override;
	virtual void ApplySaveData(FSavedActorData& InSaveData) override;
#pragma endregion
	
#pragma region Death
	void OnDeath();
#pragma endregion
	
#pragma region Tool
	FORCEINLINE APlayerTool* GetCurrentTool() const { return CurrentTool; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentToolByTag(const FGameplayTag& ToolTag);
	void InitializeCurrentTool();
#pragma endregion
	
#pragma region Widget
	void InitializeWidgets() const;
	void DisconnectWidgets() const;
#pragma endregion
	
#pragma region Overrides
public:
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& Unused, float& VerticalLocation) override;
	virtual void NotifyControllerChanged() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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

	void Input_OnAttack();

	void Input_OnInteraction();
#pragma endregion

#pragma region Components
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UAlsCameraComponent> Camera;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<USkeletalMeshComponent> VisibleMesh;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UWeaponTraceComponent> WeaponTrace;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UPlayerInputBufferComponent> PlayerInputBuffer;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UPlayerCharacterStatusComponent> Status;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UInteractionComponent> Interaction;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UGridDetectionComponent> GridDetection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UEnvironmentalStatusComponent> EnvironmentalStatus;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Player Character")
	TObjectPtr<UPlayerInventoryComponent> PlayerInventory;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Player Character", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> InteractionAction;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Cost")
	FPlayerCharacterSettings PlayerCharacterSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Data")
	TObjectPtr<UPlayerCharacterSettingsDataAsset> PlayerCharacterSettingsDataAsset;
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

#pragma region Cached
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool")
	TObjectPtr<APlayerTool> CurrentTool;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	FName UniqueSaveID;
#pragma endregion
	
};
