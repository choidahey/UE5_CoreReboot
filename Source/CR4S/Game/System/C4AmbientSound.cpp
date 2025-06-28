#include "Game/System/C4AmbientSound.h"
#include "Game/System/AudioManager.h"
#include "Components/AudioComponent.h"

void AC4AmbientSound::BeginPlay()
{
	Super::BeginPlay();

	CachedAudioComponent = GetAudioComponent();

	if (UAudioManager* AudioManager = GetWorld()->GetGameInstance()->GetSubsystem<UAudioManager>())
	{
		const float FinalVolume =
			AudioManager->GetMasterVolume() * AudioManager->GetBGMVolume() * VolumeMultiplier;

		if (IsValid(CachedAudioComponent))
		{
			CachedAudioComponent->SetVolumeMultiplier(FinalVolume);

			if (bAutoPlayOnBegin && !CachedAudioComponent->IsPlaying())
			{
				CachedAudioComponent->Play();
			}
		}

		AudioManager->OnSoundVolumeChanged.AddDynamic(this, &AC4AmbientSound::HandleVolumeChanged);
	}
}

void AC4AmbientSound::HandleVolumeChanged(const FSoundClassVolume& NewVolume)
{
	if (IsValid(CachedAudioComponent))
	{
		const float FinalVolume = NewVolume.Master * NewVolume.BGM * VolumeMultiplier;
		CachedAudioComponent->SetVolumeMultiplier(FinalVolume);
	}
}

void AC4AmbientSound::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UAudioManager* AudioManager = GetGameInstance()->GetSubsystem<UAudioManager>())
	{
		AudioManager->OnSoundVolumeChanged.RemoveDynamic(this, &AC4AmbientSound::HandleVolumeChanged);
	}

	Super::EndPlay(EndPlayReason);
}
