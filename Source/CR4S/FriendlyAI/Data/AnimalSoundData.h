#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "AnimalSoundData.generated.h"

UCLASS(BlueprintType)
class CR4S_API UAnimalSoundData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<USoundBase*> FootstepSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<USoundBase*> WingFlapSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	TArray<USoundBase*> JumpSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<USoundBase*> HitSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<USoundBase*> DeathSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<USoundBase*> MeleeSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<USoundBase*> ChargeSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<USoundBase*> RangedSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Voice")
	TArray<USoundBase*> VoiceSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	TArray<USoundBase*> InteractionSounds;
};