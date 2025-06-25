#include "Game/System/AudioManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "GameFramework/GameUserSettings.h"
#include "Game/SaveGame/SettingsSaveGame.h"
#include "Game/SaveGame/SaveGameManager.h"
#include "Components/AudioComponent.h"

void UAudioManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Collection.InitializeDependency<USaveGameManager>();

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
	USettingsSaveGame* SaveGame = NewObject<USettingsSaveGame>();
	SaveGame->MasterVolume = MasterVolume;
	SaveGame->BGMVolume = BGMVolume;
	SaveGame->SFXVolume = SFXVolume;

	UGameplayStatics::SaveGameToSlot(SaveGame, TEXT("SettingsSave"), 0);
}

void UAudioManager::LoadVolumeSettings()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>())
		{
			if (USettingsSaveGame* SaveGame = SaveGameManager->LoadSettings())
			{
				SetMasterVolume(SaveGame->MasterVolume);
				SetBGMVolume(SaveGame->BGMVolume);
				SetSFXVolume(SaveGame->SFXVolume);

				UE_LOG(LogTemp, Log, TEXT("[AudioManager] Volume loaded - Master: %.2f, BGM: %.2f, SFX: %.2f"),
					SaveGame->MasterVolume, SaveGame->BGMVolume, SaveGame->SFXVolume);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[AudioManager] Failed to load settings."));
			}
		}
	}
}

void UAudioManager::Play2DSound(USoundBase* Sound, float VolumeMultiplier, float PitchMultiplier, float StartTime)
{
	if (!Sound) return;

	UGameplayStatics::SpawnSound2D(GetWorld(), Sound, VolumeMultiplier, PitchMultiplier, StartTime);
}

UAudioComponent* UAudioManager::PlayBGM(USoundBase* BGM)
{
	if (!BGM) return nullptr;

	if (CurrentBGMComponent && CurrentBGMComponent->IsPlaying())
	{
		CurrentBGMComponent->FadeOut(1.0f, 0.0f);
	}

	CurrentBGMComponent = UGameplayStatics::SpawnSound2D(GetWorld(), BGM, BGMVolume, 1.0f, 0.0f, nullptr, true);
	if (CurrentBGMComponent)
	{
		CurrentBGMComponent->FadeIn(1.5f, 1.0f);
		SetMasterVolume(MasterVolume);
		SetBGMVolume(BGMVolume);
		SetSFXVolume(SFXVolume);
		UE_LOG(LogTemp, Log, TEXT("Set BGM Volume to: %.2f"), BGMVolume);
	}

	return CurrentBGMComponent;
}

void UAudioManager::StopBGM()
{
	if (CurrentBGMComponent && CurrentBGMComponent->IsPlaying())
	{
		CurrentBGMComponent->FadeOut(1.0f, 0.0f);
		CurrentBGMComponent = nullptr;
	}
}
