// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTool.h"
#include "Character/Data/WeaponData.h"
#include "PlayerTool.generated.h"

class UWeaponTraceComponent;
class APlayerCharacter;

UCLASS()
class CR4S_API APlayerTool : public ABaseTool
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlayerTool();

	void Initialize(APlayerCharacter* OwnerCharacter);
	
#pragma region Override
public:
	virtual void SetGameplayTag(const FGameplayTag GameplayTag) override;
	virtual AActor* GetToolOwner() const override { return Owner; } 
	virtual UMeshComponent* GetToolMeshComponent() override;
	virtual void OnAttack() override;
	virtual float ComputeFinalDamage() override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region Components
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
#pragma endregion

#pragma region Info
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FPlayerToolInfo PlayerToolInfo;
#pragma endregion
	
#pragma region Cached
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<APlayerCharacter> OwningCharacter;
#pragma endregion
};
