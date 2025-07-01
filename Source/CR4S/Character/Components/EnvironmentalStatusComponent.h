#pragma once

#include "Components/ActorComponent.h"
#include "Game/System/WorldTimeManager.h"
#include "Game/System/SeasonType.h"
#include "EnvironmentalStatusComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterHumidityChanged, float, NewHumidity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterTemperatureChanged, float, NewTemperature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTemperatureBreach, int32, BreachCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHumidityBreach, int32, BreachCode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTemperatureNormalized);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHumidityNormalized);

class AEnvironmentManager;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CR4S_API UEnvironmentalStatusComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void Refresh();
protected:
	UEnvironmentalStatusComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeTimeSystemBindings();

#pragma region Getters & Setters
public:
	FORCEINLINE float GetCurrentTemperature() const { return CurrentTemperature; }
	FORCEINLINE float GetCurrentHumidity() const { return CurrentHumidity; }
	FORCEINLINE float GetMaxTemperature() const { return MaxTemperature; }
	FORCEINLINE float GetMinTemperature() const { return MinTemperature; }
	FORCEINLINE float GetMaxHumidity() const { return MaxHumidity; }
	FORCEINLINE float GetMinHumidity() const { return MinHumidity; }

	void SetCurrentTemperature(const float NewValue);
	void SetCurrentHumidity(const float NewValue);
	FORCEINLINE void SetMaxTemperature(float Value) { MaxTemperature = Value; }
	FORCEINLINE void SetMinTemperature(float Value) { MinTemperature = Value; }
	FORCEINLINE void SetMaxHumidity(float Value) { MaxHumidity = Value; }
	FORCEINLINE void SetMinHumidity(float Value) { MinHumidity = Value; }

	float GetBaseTemperatureBySeason(ESeasonType Season) const;
	float GetBaseHumidityBySeason(ESeasonType Season) const;

	UFUNCTION(BlueprintCallable, Category = "EnvironmentalStatus")
	void ModifyTemperature(float Delta, float Speed);
	UFUNCTION(BlueprintCallable, Category = "EnvironmentalStatus")
	void ModifyHumidity(float Delta, float Speed);



#pragma endregion

#pragma region Utility Functions

	UFUNCTION()
	void UpdateTemperatureByMinute(int32 Minute, int32 DayCycle);
	UFUNCTION()
	void UpdateDayLengthChanged(float DawnTime, float DuskTime);
	UFUNCTION()
	void HandleSeasonChanged(ESeasonType Season);

	void UpdateTemperature(float DeltaTime, float Speed);
	void UpdateHumidity(float DeltaTime, float Speed);

	bool ShouldDisableTick();

#pragma endregion

#pragma region Environment Parameters

private:
	UPROPERTY(VisibleAnywhere)
	float CurrentTemperature = 0.0f;
	UPROPERTY(VisibleAnywhere)
	float CurrentHumidity = 0.0f;

	float CurrentSeasonBaseTemperature = 0.0f;
	float CurrentSeasonBaseHumidity = 0.0f;

	float MinTemperature = -20.f;
	float MaxTemperature = 30.f;

	float MinHumidity = 0.f;
	float MaxHumidity = 100.f;

	float TargetHumidity = 0.0f;
	float TargetTemperature = 0.0f;
	
	float ExternalTemperatureDelta = 0.0f;
	float ExternalHumidityDelta = 0.0f;

	float TemperatureOffset = 0.0f;

	float TemperatureChangeSpeed = 0.5f;
	float HumidityChangeSpeed = 0.5f;

	float CurrentDawnTime = 600.0f;
	float CurrentDuskTime = 1600.0f;
	float DayMid = 1200.0f;
	float NightMid = 0.0f;

	TWeakObjectPtr<AEnvironmentManager> EnvironmentManager;

	bool bWasTemperatureInRange = true;
	bool bWasHumidityInRange = true;

#pragma endregion
#pragma region Delegates and Delegate Parameters
public:
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnCharacterTemperatureChanged OnTemperatureChanged;

	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnCharacterHumidityChanged OnHumidityChanged;
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnTemperatureBreach OnTemperatureBreach;
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnHumidityBreach OnHumidityBreach;
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnTemperatureNormalized OnTemperatureNormalized;
	UPROPERTY(BlueprintAssignable, Category = "Environment")
	FOnHumidityNormalized OnHumidityNormalized;

private:
	int32 DayCycleLength;
	float CurrentDayNightRatio = 0.5f;



#pragma endregion


};
