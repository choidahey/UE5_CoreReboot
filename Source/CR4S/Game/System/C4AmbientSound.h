#pragma once

#include "Sound/AmbientSound.h"
#include "C4AmbientSound.generated.h"

UCLASS()
class CR4S_API AC4AmbientSound : public AAmbientSound
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "C4 Ambient Sound")
	bool bAutoPlayOnBegin = true;

	UPROPERTY(EditAnywhere, Category = "C4 Ambient Sound")
	float VolumeMultiplier = 1.0f;
};
