#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "AudioManager.generated.h"

USTRUCT(BlueprintType)
struct FSoundClassVolume //Multiplier
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	float Master = 1.0f;
	UPROPERTY(BlueprintReadWrite)
	float BGM = 1.0f;
	UPROPERTY(BlueprintReadWrite)
	float SFX = 1.0f;
};

UENUM(BlueprintType)
enum class EConcurrencyType : uint8
{
	UI         UMETA(DisplayName = "UI"),			// eg. button click, button hover
	Impact     UMETA(DisplayName = "Impact"),	    // eg. explosion
	AI         UMETA(DisplayName = "AI"),			// eg. AI voice, Monster cry
	Repetition UMETA(DisplayName = "Repetition"),	// eg. footsteps, gun shot
	Default    UMETA(DisplayName = "Default")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoundVolumeChanged, const FSoundClassVolume&, NewVolume);

UCLASS()
class CR4S_API UAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FORCEINLINE float GetMasterVolume() const { return GameSoundVolume.Master; }
	FORCEINLINE float GetBGMVolume() const { return GameSoundVolume.BGM; }
	FORCEINLINE float GetSFXVolume() const { return GameSoundVolume.SFX; }

	void SetMasterVolume(float Volume);
	void SetBGMVolume(float Volume);
	void SetSFXVolume(float Volume);

	//void SaveVolumeSettings();
	void LoadVolumeSettings();

#pragma region Play Sound Functions

public:
	// Plays Background Music //
	// Before playing a new BGM, it fades out and stops the BGM that has been playing
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayBGM(USoundBase* BGM);

	// Plays Sound Effect in a 3D Space//
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlaySFX(USoundBase* SFX, FVector Location, EConcurrencyType SoundType, float Pitch = 1.0f, float StartTime = 0.0f);

	// Plays 2D UI Sound Effects  //
	UFUNCTION(BlueprintCallable)
	UAudioComponent* PlayUISound(USoundBase* Sound, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

	// Stops Current BGM that is being played //
	UFUNCTION(BlueprintCallable)
	void StopBGM();


protected:

	UPROPERTY()
	UAudioComponent* CurrentBGMComponent;

#pragma endregion 

#pragma region Sound Classes and Mix
protected:
	UPROPERTY()
	USoundMix* MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* MasterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* BGMClass;
	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* SFXClass;

#pragma endregion 

#pragma region Concurrencies
	UPROPERTY()
	USoundConcurrency* BGMConcurrency;
	UPROPERTY()
	USoundConcurrency* UIConcurrency;
	UPROPERTY()
	USoundConcurrency* ImpactConcurrency;
	UPROPERTY()
	USoundConcurrency* AIConcurrency;
	UPROPERTY()
	USoundConcurrency* RepetitionConcurrency;

#pragma endregion 
public:

	UPROPERTY(BlueprintAssignable)
	FOnSoundVolumeChanged OnSoundVolumeChanged;


private:
	UFUNCTION(BlueprintCallable)
	USoundConcurrency* GetConcurrencyByType(EConcurrencyType SoundType) const;

	FSoundClassVolume GameSoundVolume;

	bool bMixPushed = false;
};