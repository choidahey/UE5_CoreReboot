#pragma once

#include "Components/ActorComponent.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonType.h"
#include "EnvironmentalStatusComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterHumidityChanged, float, NewHumidity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterTemperatureChanged, float, NewTemperature);

class AEnvironmentManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CR4S_API UEnvironmentalStatusComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UEnvironmentalStatusComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BindToTimeSystemDelegates();



#pragma region Getters & Setters
public:
	FORCEINLINE float GetCurrentTemperature() const { return CurrentTemperature; }
	FORCEINLINE float GetCurrentHumidity() const { return CurrentHumidity; }
	FORCEINLINE float GetMaxTemperature() const { return MaxTemperature; }
	FORCEINLINE float GetMinTemperature() const { return MinTemperature; }
	FORCEINLINE float GetMaxHumidity() const { return MaxHumidity; }
	FORCEINLINE float GetMinHumidity() const { return MinHumidity; }

	void ModifyTemperature(float Delta, float Speed);
	void ModifyHumidity(float Delta, float Speed);

	void SetMaxTemperature(float Max);
	void SetMinTemperature(float Min);
	void SetMaxHumidity(float Max);


#pragma endregion

#pragma region Utility Functions

	UFUNCTION()
	void UpdateTemperatureByMinute(int32 Minute);
	UFUNCTION()
	void UpdateDayNightRatio(float Ratio);
	UFUNCTION()
	void HandleSeasonChanged(ESeasonType Season);

	float GetBaseTemperatureBySeason(ESeasonType Season) const;

	void UpdateTemperature(float DeltaTime, float Speed);

	bool IsPlayer() const;

#pragma endregion

#pragma region Environment Parameters

private:
	float CurrentTemperature = 0.0f;
	float CurrentHumidity = 0.0f;

	float CurrentSeasonBaseTemperature = 0.0f;

	float MinTemperature = -20.f;
	float MaxTemperature = 30.f;

	float MinHumidity = 0.f;
	float MaxHumidity = 100.f;

	float TargetHumidity = 0.0f;
	float TargetTemperature = 0.0f;

	float TemperatureChangeSpeed = 0.5f;
	float HumidityChangeSpeed = 0.5f;


	TWeakObjectPtr<AEnvironmentManager> EnvironmentManager;

#pragma endregion
#pragma region Delegates and Delegate Parameters
public:
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnCharacterTemperatureChanged OnTemperatureChanged;

	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnCharacterHumidityChanged OnHumidityChanged;


private:
	int32 DayCycleLength;
	float CurrentDayNightRatio = 0.5f;

#pragma endregion


};
