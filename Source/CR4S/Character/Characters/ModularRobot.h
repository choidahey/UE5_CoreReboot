// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Data/RobotSettings.h"
#include "Game/Interface/SavableActor.h"
#include "Game/SaveGame/CoreSaveGame.h"
#include "GameFramework/Character.h"
#include "Utility/StunnableInterface.h"
#include "ModularRobot.generated.h"

class USaveGame;
class UTimelineComponent;
class UDataLoaderSubsystem;
struct FGameplayTag;
class URobotInventoryComponent;
class URobotInputBufferComponent;
class UInputBufferComponent;
class URobotWeaponComponent;
class UModularRobotStatusComponent;
class APlayerCharacter;
class UInteractableComponent;
class UEnvironmentalStatusComponent;
class UGridDetectionComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class CR4S_API AModularRobot : public ACharacter, public IStunnableInterface, public ISavableActor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AModularRobot();

#pragma region Save/Load
	virtual FName GetUniqueSaveID() override;
	virtual void SetUniqueSaveID(FName NewID) override;
	virtual void GatherSaveData(FSavedActorData& OutSaveData) override;
	virtual void ApplySaveData(FSavedActorData& InSaveData) override;
#pragma endregion

#pragma region InputEnable
	void SetInputEnable(const bool bEnableInput) const;
	void SetMovementInputEnable(const bool bEnableMovementInput) const;
#pragma endregion

#pragma region PartsEquip
	UFUNCTION(BlueprintCallable)
	void EquipCoreParts(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable)
	void EquipBodyParts(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable)
	void EquipArmParts(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable)
	void EquipLegParts(const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable)
	void EquipBoosterParts(const FGameplayTag& Tag);
	
	UFUNCTION(BlueprintCallable)
	void UnequipCoreParts();
	UFUNCTION(BlueprintCallable)
	void UnequipBodyParts();
	UFUNCTION(BlueprintCallable)
	void UnequipArmParts();
	UFUNCTION(BlueprintCallable)
	void UnequipLegParts();
	UFUNCTION(BlueprintCallable)
	void UnequipBoosterParts();

	void UnequipAll();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLegManagerEnabled(const bool bIsEnabled);
#pragma endregion

#pragma region Stun
	virtual void TakeStun_Implementation(const float StunAmount) override;
#pragma endregion

#pragma region Get
	FORCEINLINE APlayerCharacter* GetMountedCharacter() const { return MountedCharacter; }
	FORCEINLINE bool IsRobotActive() const { return Status->IsRobotActive(); }
	FORCEINLINE float GetRecoilModifier() const { return Status->GetRecoilModifier(); }
	FORCEINLINE UModularRobotStatusComponent* GetStatusComponent() const { return Status; }
	FORCEINLINE URobotWeaponComponent* GetWeaponComponent() const { return WeaponManager; }
	FORCEINLINE bool EquippedBodyParts() const { return BodyTag != FGameplayTag::EmptyTag;}
#pragma endregion

#pragma region Death
	void OnDeath();
#pragma endregion

#pragma region Load Data
	UDataLoaderSubsystem* GetDataLoaderSubsystem() const;
	void LoadDataFromDataLoader();
#pragma endregion

#pragma region Mount
	UFUNCTION(BlueprintCallable)
	void MountRobot(AActor* InActor);
	UFUNCTION(BlueprintCallable)
	void UnMountRobot();

	bool FindPossibleUnmountLocation(ACharacter* CharacterToDrop, FVector& OutLocation) const;
#pragma endregion

#pragma region Widgets
	void InitializeWidgets() const;
	void DisconnectWidgets() const;
#pragma endregion

#pragma region OverrideFunctions

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void Destroyed() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void NotifyControllerChanged() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
#pragma endregion

#pragma region MoveFunctions

protected:
	UFUNCTION()
	void Input_Move(const FInputActionValue& Value);
	UFUNCTION()
	void Input_Look(const FInputActionValue& Value);
	UFUNCTION()
	void Input_StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void Input_StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Input_Dash(const FInputActionValue& Value);

	UFUNCTION()
	void ResetDashCooldown();

	UFUNCTION()
	void OnHoverTimeLineUpdate(float Value);
#pragma endregion

#pragma region InputActions

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack1Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack2Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Movement", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack4Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Utility", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputMappingContext> UtilityMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMC | Utility", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> InteractionAction;
#pragma endregion

#pragma region Settings

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FRobotSettings RobotSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FRobotSettings DefaultSettings;
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
	TObjectPtr<URobotWeaponComponent> WeaponManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractableComponent> InteractComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGridDetectionComponent> GridDetection;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnvironmentalStatusComponent> EnvironmentalStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URobotInputBufferComponent> InputBuffer;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URobotInventoryComponent> RobotInventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ArmMesh;
#pragma endregion

#pragma region Cached

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<APlayerCharacter> MountedCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle DashCooldownTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag CoreTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag BodyTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag ArmTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag LegTag;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag BoosterTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDashing : 1 {false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsHovering : 1 {false};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SaveGame")
	FName UniqueSaveID;
#pragma endregion

#pragma region Hover
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTimelineComponent> HoverTimeLine;
#pragma endregion

#pragma region DebugOption
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	uint8 bIsDebugMode : 1 {false};
#pragma endregion
};
