// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class UPlayerCharacterStatus;
class APlayerCharacter;

UENUM(BlueprintType)
enum class EInputType : uint8
{
	None			UMETA(DisplayName = "None"),
	Attack			UMETA(DisplayName = "Attack"),
	RobotAttack1	UMETA(DisplayName = "RobotAttack1"),
	RobotAttack2	UMETA(DisplayName = "RobotAttack2")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();

#pragma region GetSet
	void SetTopSocketName(const FName InSocketName);
	void SetBottomSocketName(const FName InSocketName);
#pragma endregion	

#pragma region Input & Weapon
	virtual void SetWeaponTrace(const bool Trace);
	virtual void ExecuteInputQueue();
	virtual void PerformWeaponTrace();
	void SetInputEnable(const bool Enable);
	void SetInputQueue(const EInputType Input);
	bool CheckInputQueue(const EInputType Input);
	void SweepAndApplyDamage(AActor* OwningCharacter, const FVector& CurrentTop, const FVector& CurrentBottom, const float InDamage);
#pragma endregion
	
#pragma region OverrideFunctions
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Settings
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Settings")
	uint8 bDebugMode:1 {false};
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Settings")
	FName TopSocketName {"Top"};
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Settings")
	FName BottomSocketName {"Bottom"};
#pragma endregion
	
#pragma region Cached
	uint8 bInputEnable:1 {true};
	uint8 bWeaponTrace:1 {false};
	FVector PreviousTopLocation {};
	FVector PreviousBottomLocation {};
	UPROPERTY()
	TSet<AActor*> AlreadyDamagedActors;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Input")
	EInputType CurrentInputQueue {EInputType::None};
#pragma endregion

#pragma region Buffer
private:
	void ClearInputQueue();
	
	FTimerHandle BufferClearTimerHandle;
#pragma endregion
};

