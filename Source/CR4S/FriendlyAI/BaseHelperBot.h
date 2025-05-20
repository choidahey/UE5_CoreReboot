#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HelperBotState.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "BaseHelperBot.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API ABaseHelperBot : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseHelperBot();

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UMaterialInstanceDynamic* HighlightMaterialInstance;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UInteractableComponent> InteractableComp;

	UFUNCTION()
	void HandleInteract();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHelperBotStateManagerWidget> StateUIClass;

	UHelperBotStateManagerWidget* StateUIInstance;
};