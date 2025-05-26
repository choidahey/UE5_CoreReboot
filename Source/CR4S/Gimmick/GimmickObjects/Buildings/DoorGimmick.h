#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleResourceGimmick.h"

#include "DoorGimmick.generated.h"

class UInteractableComponent;

UCLASS()
class CR4S_API ADoorGimmick : public ADestructibleResourceGimmick
{
	GENERATED_BODY()


#pragma region ADestructibleResourceGimmick Override

public:
	ADoorGimmick();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	UFUNCTION()
	virtual void OnGimmickInteracted(AController* Controller);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;

private:
	void UpdateInteractionText() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionTextOpen;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionTextClose;

#pragma endregion
	
#pragma region Door

private:
	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bIsMoving;
	
	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bIsOpen;
	
	UPROPERTY(VisibleAnywhere, Category = "Door")
	bool bNextStateIsOpen;
	
	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenAngle;
	UPROPERTY(EditAnywhere, Category = "Door")
	float DoorSpeed;
	
	FRotator ClosedRotation;
	FRotator OpenRotation;
	FRotator TargetRotation;
	
#pragma endregion
};
