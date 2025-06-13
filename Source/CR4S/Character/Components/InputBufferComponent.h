// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputBufferComponent.generated.h"


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
class CR4S_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInputBufferComponent();

#pragma region Input & Weapon
	virtual void ExecuteInputQueue() const;
	void SetInputEnable(const bool Enable);
	void SetInputQueue(const EInputType Input);
	bool CheckInputQueue(const EInputType Input);
	void ClearInputQueue();
#pragma endregion
	
#pragma region OverrideFunctions
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region Cached
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Input")
	uint8 bInputEnable:1 {true};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Input")
	EInputType CurrentInputQueue {EInputType::None};
#pragma endregion

#pragma region Buffer
private:
	FTimerHandle BufferClearTimerHandle;
#pragma endregion
};

