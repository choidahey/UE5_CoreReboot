// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Components/ModularRobotStatusComponent.h"
#include "Character/Data/RobotPartsData.h"
#include "Character/Data/RobotSettings.h"
#include "GameFramework/Character.h"
#include "Utility/StunnableInterface.h"
#include "ModularRobot.generated.h"

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
class CR4S_API AModularRobot : public ACharacter, public IStunnableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AModularRobot();

#pragma region PartsEquip
	void EquipCoreParts(const FGameplayTag& Tag);
	void EquipBodyParts(const FGameplayTag& Tag);
	void EquipArmParts(const FGameplayTag& Tag);
	void EquipLegParts(const FGameplayTag& Tag);
	void EquipBoosterParts(const FGameplayTag& Tag);

	void UnequipCoreParts();
	void UnequipBodyParts();
	void UnequipArmParts();
	void UnequipLegParts();
	void UnequipBoosterParts();

	UFUNCTION(BlueprintImplementableEvent)
	void SetLegManagerEnabled(const bool bIsEnabled);
#pragma endregion
	
#pragma region Stun
	virtual void TakeStun_Implementation(const float StunAmount) override;
	void SetInputEnable(const bool bEnableInput);
#pragma endregion
	
#pragma region Get
	FORCEINLINE APlayerCharacter* GetMountedCharacter() const { return MountedCharacter; }
	FORCEINLINE bool IsRobotActive() const { return Status->IsRobotActive(); }
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
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
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
	void Input_HorizontalDash(const FInputActionValue& Value);
	UFUNCTION()
	void Input_VerticalDash(const FInputActionValue& Value);

	UFUNCTION()
	void ResetDashCooldown();
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
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> HorizontalDashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> VerticalDashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> InteractionAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack1Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack2Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack3Action;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings|Modular Robot", Meta = (DisplayThumbnail = false))
	TObjectPtr<UInputAction> Attack4Action;
	
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
	FCorePartsInfo CoreInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FBodyPartsInfo BodyInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FArmPartsInfo ArmInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FLegPartsInfo LegInfo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FBoosterPartsInfo BoosterInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float OriginalMaxWalkSpeed{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float OriginalDashStrength{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float OriginalMaxSlopeAngle{0};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float OriginalDashCooldown{0};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsDashing:1 {false};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	uint8 bIsHovering:1 {false};
#pragma endregion

#pragma region DebugOption
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	uint8 bIsDebugMode:1 {false};
#pragma endregion
};

