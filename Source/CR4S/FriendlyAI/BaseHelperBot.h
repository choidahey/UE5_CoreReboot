#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HelperBotState.h"
#include "BaseHelperBot.generated.h"

UCLASS()
class CR4S_API ABaseHelperBot : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseHelperBot();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	EHelperBotState GetBotState() const { return CurrentState; }
	
	void SetBotState(EHelperBotState NewState);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EHelperBotState CurrentState;

};
