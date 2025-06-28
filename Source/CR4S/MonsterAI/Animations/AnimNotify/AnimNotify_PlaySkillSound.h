#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Game/System/AudioManager.h"
#include "AnimNotify_PlaySkillSound.generated.h"

UCLASS()
class CR4S_API UAnimNotify_PlaySkillSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TSoftObjectPtr<USoundBase> Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	EConcurrencyType ConcurrencyType = EConcurrencyType::Impact;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound",meta = (ClampMin = "0.0"))
	float PitchMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound",meta = (ClampMin = "0.0"))
	float StartTime = 0.f;
};
