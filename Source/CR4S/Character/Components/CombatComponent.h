// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class UPlayerCharacterStatus;
class APlayerCharacter;

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
#pragma endregion

#pragma region InputState
	void SetInputEnable(bool Enable);
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

#pragma region Cached
	uint8 bInputEnable:1;
#pragma endregion
};
