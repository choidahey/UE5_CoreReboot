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

	// Load Sound Classes and Mix
	MasterSoundMix = LoadObject<USoundMix>(nullptr, TEXT("/Game/CR4S/_Sounds/SCM_CR4S.SCM_CR4S"));

	MasterSoundClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_Master.SC_Master"));
	BGMClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_BGM.SC_BGM"));
	SFXClass = LoadObject<USoundClass>(nullptr, TEXT("/Game/CR4S/_Sounds/SC_SFX.SC_SFX"));

	// Load Concurrencies
	BGMConcurrency = LoadObject<USoundConcurrency>(nullptr, TEXT("/Game/CR4S/_Sounds/Concurrency/SCON_BGM_Ambient.SCON_BGM_Ambient"));
	UIConcurrency = LoadObject<USoundConcurrency>(nullptr, TEXT("/Game/CR4S/_Sounds/Concurrency/SCON_UI.SCON_UI"));
	ImpactConcurrency = LoadObject<USoundConcurrency>(nullptr, TEXT("/Game/CR4S/_Sounds/Concurrency/SCON_Impact.SCON_Impact"));
	AIConcurrency = LoadObject<USoundConcurrency>(nullptr, TEXT("/Game/CR4S/_Sounds/Concurrency/SCON_AI.SCON_AI"));
	RepetitionConcurrency = LoadObject<USoundConcurrency>(nullptr, TEXT("/Game/CR4S/_Sounds/Concurrency/SCON_Repetition.SCON_Repetition"));

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
	//if (MasterSoundMix && MasterSoundClass)
	//{
	//	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MasterSoundClass, Volume);
	//}

	GameSoundVolume.Master = Volume;

	if (CurrentBGMComponent)
	{
		CurrentBGMComponent->SetVolumeMultiplier(GameSoundVolume.BGM * GameSoundVolume.Master);
	}
}

void UAudioManager::SetBGMVolume(float Volume)
{
	//if (MasterSoundMix && BGMClass)
	//{
	//	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, BGMClass, Volume);
	//}

	GameSoundVolume.BGM = Volume;

	if (CurrentBGMComponent)
	{
		CurrentBGMComponent->SetVolumeMultiplier(GameSoundVolume.BGM * GameSoundVolume.Master);
	}
}

void UAudioManager::SetSFXVolume(float Volume)
{
	//if (MasterSoundMix && SFXClass)
	//{
	//	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, SFXClass, Volume);
	//}

	GameSoundVolume.SFX = Volume;
}

void UAudioManager::LoadVolumeSettings()
{
	if (USaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<USaveGameManager>())
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

	CurrentBGMComponent = UGameplayStatics::SpawnSound2D(
		GetWorld(),
		BGM,
		1.0f, // volume
		1.0f, // pitch
		0.0f, // start time
		BGMConcurrency, //concurrency
		true
	);

	if (CurrentBGMComponent)
	{
		const float TargetVolume = GameSoundVolume.BGM * GameSoundVolume.Master;
		CurrentBGMComponent->FadeIn(1.5f);
		SetBGMVolume(GameSoundVolume.BGM);

		UE_LOG(LogTemp, Log, TEXT("Set BGM Volume to: %.2f (Effective: %.2f)"), GameSoundVolume.BGM, TargetVolume);
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

UAudioComponent* UAudioManager::PlaySFX(USoundBase* SFX, FVector Location, EConcurrencyType SoundType, float Pitch, float StartTime)
{
	if (!SFX) return nullptr;

	const float EffectiveVolume = GameSoundVolume.SFX * GameSoundVolume.Master;
	USoundConcurrency* Concurrency = GetConcurrencyByType(SoundType);

	UAudioComponent* SFXComponent = UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(),
		SFX,
		Location,
		FRotator::ZeroRotator,
		EffectiveVolume,
		Pitch,
		StartTime,
		nullptr,
		Concurrency
	);

	UE_LOG(LogTemp, Verbose, TEXT("[AudioManager] PlaySFX [%s] at %s | Volume: %.2f"),
		*UEnum::GetValueAsString(SoundType),
		*Location.ToCompactString(),
		EffectiveVolume);

	return SFXComponent;
}
// AudioComponent->SetOwner(ptr);


USoundConcurrency* UAudioManager::GetConcurrencyByType(EConcurrencyType SoundType) const
{
	switch (SoundType)
	{
	case EConcurrencyType::UI:
		return UIConcurrency;
	case EConcurrencyType::Impact:
		return ImpactConcurrency;
	case EConcurrencyType::AI:
		return AIConcurrency;
	case EConcurrencyType::Repetition:
		return RepetitionConcurrency;
	default:
		return nullptr;
	}
}
