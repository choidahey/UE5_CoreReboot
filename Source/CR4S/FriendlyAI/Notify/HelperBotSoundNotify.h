#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Game/System/AudioManager.h"
#include "HelperBotSoundNotify.generated.h"

UCLASS()
class CR4S_API UHelperBotSoundNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UHelperBotSoundNotify();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta=(GetOptions="GetSoundOptions"))
	FString SoundPropertyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	EConcurrencyType ConcurrencyType = EConcurrencyType::AI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float Pitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float StartTime = 0.0f;

private:
	UFUNCTION()
	TArray<FString> GetSoundOptions() const;
};