#include "Game/System/AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "GameFramework/GameUserSettings.h"
#include "AudioManager.h"

void UAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	MasterSoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/CR4S/_Sounds/SCM_CR4S.SCM_CR4S"));

	MasterSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_Master.SC_Master"));
	BGMClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_BGM.SC_BGM"));
	SFXClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_SFX.SC_SFX"));

	UE_LOG(LogTemp, Warning, TEXT("MasterSoundMix: %s"), *GetNameSafe(MasterSoundMix));
	UE_LOG(LogTemp, Warning, TEXT("MasterSoundClass: %s"), *GetNameSafe(MasterSoundClass));
	UE_LOG(LogTemp, Warning, TEXT("BGMClass: %s"), *GetNameSafe(BGMClass));
	UE_LOG(LogTemp, Warning, TEXT("SFXClass: %s"), *GetNameSafe(SFXClass));

	if (MasterSoundMix && !bMixPushed)
	{
		UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
		bMixPushed = true;
		UE_LOG(LogTemp, Warning, TEXT("SoundMix pushed successfully."));
	}

	LoadVolumeSettings();
}


void UAudioManager::SetMasterVolume(float Volume)
{
	if (MasterSoundMix && MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MasterSoundClass, Volume);

		MasterVolume = Volume;
	}
}

void UAudioManager::SetBGMVolume(float Volume)
{
	if (MasterSoundMix && BGMClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, BGMClass, Volume);

		BGMVolume = Volume;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Missing MasterSoundMix or BGMClass"));
	}

}

void UAudioManager::SetSFXVolume(float Volume)
{
	if (MasterSoundMix && SFXClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, SFXClass, Volume);

		SFXVolume = Volume;
	}
}

void UAudioManager::SaveVolumeSettings()
{
	// Use SaveGame to persist volume settings

}

void UAudioManager::LoadVolumeSettings()
{
	// Use Save Game to Load volume settings
}
