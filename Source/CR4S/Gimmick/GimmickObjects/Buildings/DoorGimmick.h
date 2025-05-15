#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/BaseGimmick/CompleteGimmick.h"
#include "DoorGimmick.generated.h"

UCLASS()
class CR4S_API ADoorGimmick : public ACompleteGimmick
{
	GENERATED_BODY()


#pragma region ACompleteGimmick Override

public:
	ADoorGimmick();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void OnGimmickDestroyed() override;
	virtual void OnGimmickInteracted() override;
	
#pragma endregion

#pragma region Door

private:
	void UpdateInteractionText() const;
	
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

#pragma region Interaction

private:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionTextOpen;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionTextClose;
	
#pragma endregion
};
