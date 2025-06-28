// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputBufferComponent.h"
#include "WeaponTraceComponent.generated.h"


class ABaseTool;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UWeaponTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponTraceComponent();
	
#pragma region Set
	UFUNCTION(BlueprintCallable)
	void SetCurrentTool(ABaseTool* InTool);
	void SetWeaponTrace(const bool Trace);
#pragma endregion

#pragma region Trace
	void PerformWeaponTrace();
	void SweepAndApplyDamage(AActor* OwningCharacter, const FVector& CurrentTop, const FVector& CurrentBottom, const float InDamage);
#pragma endregion

#pragma region Overrides
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
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
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<ABaseTool> CurrentTool;
private:
	uint8 bWeaponTrace:1 {false};
	FVector PreviousTopLocation {};
	FVector PreviousBottomLocation {};
	UPROPERTY()
	TSet<AActor*> AlreadyDamagedActors;
#pragma endregion
};
