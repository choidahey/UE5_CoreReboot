#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "AudioManager.generated.h"

UCLASS()
class CR4S_API UAudioManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;


	FORCEINLINE float GetMasterVolume() const { return MasterVolume; }
	FORCEINLINE float GetBGMVolume() const { return BGMVolume; }
	FORCEINLINE float GetSFXVolume() const { return SFXVolume; }



	void SetMasterVolume(float Volume);
	void SetBGMVolume(float Volume);
	void SetSFXVolume(float Volume);

	void SaveVolumeSettings();
	void LoadVolumeSettings();

protected:
	UPROPERTY()
	USoundMix* MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* MasterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* BGMClass;
	UPROPERTY(EditDefaultsOnly, Category = "SoundClass")
	USoundClass* SFXClass;

private:

	float MasterVolume = 1.0f;
	float BGMVolume = 1.0f;
	float SFXVolume = 1.0f;

	bool bMixPushed = false;
};