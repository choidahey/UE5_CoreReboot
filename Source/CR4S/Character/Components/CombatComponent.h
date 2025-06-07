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
	None	UMETA(DisplayName = "None"),
	Attack	UMETA(DisplayName = "Attack")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();
	
#pragma region Attack
public:
	void Input_OnAttack();
	void PerformWeaponTrace();
#pragma endregion

#pragma region Input
	void SetInputEnable(bool Enable);
	void SetWeaponTrace(bool Trace);
	void SetInputQueue(EInputType Input);
	bool CheckInputQueue(EInputType Input);
	void ExecuteInputQueue();
#pragma endregion
	
#pragma region OverrideFunctions
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

#pragma region AnimationMontage
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Animations")
	TObjectPtr<UAnimMontage> AttackMontage;
#pragma endregion
	
	
#pragma region Owner
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<APlayerCharacter> OwningCharacter;
#pragma endregion

#pragma region Settings
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Settings")
	uint8 bDebugMode:1 {false};
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

