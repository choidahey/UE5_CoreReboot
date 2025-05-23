#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HelperBotState.h"
#include "Data/HelperBotStatsRow.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "BaseHelperBot.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API ABaseHelperBot : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseHelperBot();

#pragma region Lifecycle
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
#pragma endregion

#pragma region Interaction Functions
	
protected:
	UFUNCTION()
	void HandleInteract(AController* InteractingController);
	
#pragma endregion

#pragma region Stats Functions
	
protected:
	void LoadStats();
	
#pragma endregion

#pragma region Interaction Variables
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> InteractableComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHelperBotStateManagerWidget> StateUIClass;

	UPROPERTY()
	TObjectPtr<UHelperBotStateManagerWidget> StateUIInstance = nullptr;
	
#pragma endregion

#pragma region Stats Variables
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName RowName = NAME_None;

	UPROPERTY()
	FHelperBotStatsRow CurrentStats = {};

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth = 0.f;
	
#pragma endregion

#pragma region Miscellaneous Variables
	
protected:
	UPROPERTY()
	UMaterialInstanceDynamic* HighlightMaterialInstance = nullptr;
	
#pragma endregion

#pragma region Getters
	
public:
	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetWoodDamagePerSecond() const { return CurrentStats.WoodDamagePerSecond; }

	UFUNCTION(BlueprintCallable, Category = "Stats")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
#pragma endregion
	
};