#pragma once

#include "Sound/AmbientSound.h"
#include "Game/System/AudioManager.h"
#include "C4AmbientSound.generated.h"

UCLASS()
class CR4S_API AC4AmbientSound : public AAmbientSound
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:

	UFUNCTION()
	void HandleVolumeChanged(const FSoundClassVolume& NewVolume);

	UPROPERTY(EditAnywhere, Category = "C4 Ambient Sound")
	bool bAutoPlayOnBegin = true;

	UPROPERTY(EditAnywhere, Category = "C4 Ambient Sound")
	float VolumeMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* CachedAudioComponent = nullptr;

};
