#pragma once

#include "CoreMinimal.h"
#include "BaseAnimal.h"
#include "AnimalFish.generated.h"

UCLASS()
class CR4S_API AAnimalFish : public ABaseAnimal
{
	GENERATED_BODY()

public:
	AAnimalFish();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyStun(float Amount) override {}
	virtual void RecoverFromStun() override {}

	UFUNCTION(BlueprintCallable, Category = "Water")
	bool IsInWater() const;

	UFUNCTION(BlueprintCallable, Category = "Water")
	FVector GetLastValidWaterPosition() const { return LastValidWaterPosition; }

	UFUNCTION(BlueprintCallable, Category = "Water")
	void SetLastValidWaterPosition(const FVector& Position) { LastValidWaterPosition = Position; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water Settings")
	float WaterBoundaryCheckDistance = 100.0f;

private:
	FVector LastValidWaterPosition;
};