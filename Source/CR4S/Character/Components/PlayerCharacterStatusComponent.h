// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/DataAsset/PlayerCharacterStatus.h"
#include "Components/ActorComponent.h"
#include "PlayerCharacterStatusComponent.generated.h"

class UPlayerCharacterStatus;
class APlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UPlayerCharacterStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerCharacterStatusComponent();
#pragma region Override
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

	
#pragma region DataAsset
protected:
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="StatusData")
	TObjectPtr<UPlayerCharacterStatus> StatusData; 
#pragma endregion

#pragma region Owner
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Owner")
	TObjectPtr<APlayerCharacter> OwningCharacter;
#pragma endregion
	
#pragma region Status
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	FPlayerCharacterStats Status;
#pragma endregion
};
