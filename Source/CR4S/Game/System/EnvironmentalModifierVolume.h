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
	void Initialize(float InRadius, float InHeight, float InTempDelta, float InHumidDelta, float InChangeSpeed, float InDuration);

protected:
	virtual void BeginPlay() override;

	

#pragma region Overlap Functions

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

#pragma endregion

#pragma region Parameters
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Temperature")
	float TemperatureDelta = -0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment|Humidity")
	float HumidityDelta = -0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment")
	float Duration = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Environment")
	float ChangeSpeed = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Height = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment")
	float Radius = 1000.0f;

#pragma endregion
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleComponent;

private:
	TMap<TObjectPtr<UEnvironmentalStatusComponent>, float> OverlappingActors;

};
