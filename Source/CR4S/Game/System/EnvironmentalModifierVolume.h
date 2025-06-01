#pragma once

#include "GameFramework/Actor.h"
#include "EnvironmentalModifierVolume.generated.h"

class USceneComponent;
class UCapsuleComponent;
class UEnvironmentalStatusComponent;

UCLASS()
class CR4S_API AEnvironmentalModifierVolume : public AActor
{
	GENERATED_BODY()

public:
	AEnvironmentalModifierVolume();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Temperature")
	float TemperatureDelta = -5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Humidity")
	float HumidityDelta = -5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment")
	float Duration = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment")
	float ChangeSpeed = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Height = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Radius = 1000.0f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

private:
	TMap<TObjectPtr<UEnvironmentalStatusComponent>, float> OverlappingActors;

public:
	virtual void Tick(float DeltaTime) override;
};
