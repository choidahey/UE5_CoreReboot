// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatComponent.h"
#include "CharacterCombatComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UCharacterCombatComponent : public UCombatComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterCombatComponent();

#pragma region Attack
	void Input_OnAttack();
	virtual void PerformWeaponTrace() override;
#pragma endregion

#pragma region Overrides
public:
	virtual void SetWeaponTrace(const bool Trace) override;
	virtual void ExecuteInputQueue() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
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
};
