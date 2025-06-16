#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerchedTarget.generated.h"

class USphereComponent;
class AAnimalFlying;

UCLASS()
class CR4S_API APerchedTarget : public AActor
{
	GENERATED_BODY()
	
public:
	APerchedTarget();

	bool GetOccupied() const { return bOccupied; }
	void SetOccupied(bool b) { bOccupied = b; }

	UFUNCTION()
	AAnimalFlying* GetCurrentOccupant() const { return CurrentOccupant; }
	
protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	bool bOccupied = false;
	
	UPROPERTY()
	AAnimalFlying* CurrentOccupant = nullptr;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};