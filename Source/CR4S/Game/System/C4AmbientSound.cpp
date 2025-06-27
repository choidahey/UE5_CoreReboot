#include "Game/System/C4AmbientSound.h"
#include "Game/System/AudioManager.h"
#include "Components/AudioComponent.h"

void AC4AmbientSound::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UAudioManager* AudioManager = World->GetGameInstance()->GetSubsystem<UAudioManager>())
		{
			const float FinalVolume =
				AudioManager->GetMasterVolume() * AudioManager->GetBGMVolume() * VolumeMultiplier;

			if (UAudioComponent* AmbientComp = GetAudioComponent())
			{
				AmbientComp->SetVolumeMultiplier(FinalVolume);

				if (bAutoPlayOnBegin && !AmbientComp->IsPlaying())
				{
					AmbientComp->Play();
				}
			}

		}
	}
}