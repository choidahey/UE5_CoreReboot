// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Character/Data/WeaponData.h"
#include "RangedWeapon.generated.h"

class UNiagaraComponent;
class UObjectPoolComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChanged, const float InPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoCountChanged, const int32, CurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxAmmoCountChanged, const int32, MaxAmmo);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartReload, const float Duration);

struct FRangedWeaponData;
class ABaseBullet;


UCLASS(BlueprintType, Blueprintable)
class CR4S_API ARangedWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARangedWeapon();

	void RefreshUI();
	
#pragma region Override
public:
	virtual void Initialize(AModularRobot* OwnerCharacter, int32 SlotIdx) override;
	virtual int32 GetCurrentAmmo() const override;
	virtual void SetCurrentAmmo(const int32 NewAmount) override;
#pragma endregion

#pragma region VFX/SFX
public:
	void PlayMuzzleVFX(const FName& MuzzleSocketName) const;
	void PlayMuzzleSFX(const FName& MuzzleSocketName) const;

	void StartMuzzleFlash(const FName& MuzzleSocketName);
	void StopMuzzleFlash();
#pragma endregion
	
#pragma region Get
	float GetCurrentAmmoPercentage() const;
	int32 GetMaxAmmo() const;
#pragma endregion
	
#pragma region Common
protected:
	void StartSequentialFire(AActor* HomingTarget = nullptr);
	UFUNCTION()
	void FireNextShotInSequence();
	
	void FireMultiBullet(AActor* HomingTarget=nullptr);
	void FireBullet(const FVector& MuzzleLocation, const FRotator& SpawnRotation, AActor* HomingTarget = nullptr);
	
	bool GetAimHitResult(FHitResult& OutHitResult) const;
	FVector GetMuzzleLocation(const FName& SocketName) const;

	void ApplyRecoil() const;
	void StartReload();
	void FinishReload();
	void AddCurrentAmmo(const int32 Amount);
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRangedWeaponInfo TypeSpecificInfo; 
#pragma endregion
	
#pragma region Cached
protected:
	FTimerHandle SequentialFireTimerHandle;
	int32 ShotsRemainingInSequence{0};
	int32 MuzzleIndexInSequence{0};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 bIsReloading:1 {false};
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TWeakObjectPtr<AActor> CurrentHomingTarget;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> MuzzleFlashComp; 
#pragma endregion

#pragma region Delegate
public:
	FOnCurrentAmmoChanged OnCurrentAmmoChanged;
	FOnStartReload OnStartReload;
	UPROPERTY(BlueprintAssignable)
	FOnCurrentAmmoCountChanged OnCurrentAmmoCountChanged;
	UPROPERTY(BlueprintAssignable)
	FOnMaxAmmoCountChanged OnMaxAmmoCountChanged;
#pragma endregion
};

