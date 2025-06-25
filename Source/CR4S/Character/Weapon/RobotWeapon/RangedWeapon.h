// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Character/Data/WeaponData.h"
#include "RangedWeapon.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentAmmoChanged, const float InPercent);

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
#pragma endregion

#pragma region Get
	float GetCurrentAmmoPercent() const;
#pragma endregion
	
#pragma region Common
protected:
	void FireMultiBullet(AActor* HomingTarget=nullptr);
	bool GetAimHitResult(FHitResult& OutHitResult) const;
	FVector GetMuzzleLocation(const FName& SocketName) const;
	void FireBullet(const FVector& MuzzleLocation, const FRotator& SpawnRotation, AActor* HomingTarget = nullptr);

	void ApplyRecoil() const;
	void StartReload();
	void FinishReload();
	void AddCurrentAmmo(const int32 Amount);
	void SetCurrentAmmo(const int32 NewAmount);
#pragma endregion
	
#pragma region TypeSpecificInfo
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRangedWeaponInfo TypeSpecificInfo; 
#pragma endregion

#pragma region Cached
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	uint8 bIsReloading:1 {false};
	FTimerHandle ReloadTimerHandle;
#pragma endregion

#pragma region Delegate
public:
	FOnCurrentAmmoChanged OnCurrentAmmoChanged;
#pragma endregion
};

