// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "BaseTool.generated.h"

UCLASS()
class CR4S_API ABaseTool : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseTool();

#pragma region GetSet
public:
	virtual AActor* GetToolOwner() const;
	virtual UMeshComponent* GetToolMeshComponent();
	virtual FVector GetSocketLocationByName(const FName SocketName);
	FORCEINLINE FGameplayTag GetToolGameplayTag() const { return ToolTag; }
	virtual void SetGameplayTag(const FGameplayTag GameplayTag);

#pragma endregion

#pragma region Common
public:
	virtual void OnAttack();
	virtual float ComputeFinalDamage();
#pragma endregion
	
#pragma region Override
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion
	
#pragma region Tag
protected:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="ToolInfo")
	FGameplayTag ToolTag;
#pragma endregion
};

