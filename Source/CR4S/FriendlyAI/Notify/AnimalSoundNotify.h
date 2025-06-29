#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Game/System/AudioManager.h"
#include "AnimalSoundNotify.generated.h"

UCLASS()
class CR4S_API UAnimalSoundNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimalSoundNotify();

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FString SoundArrayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	EConcurrencyType ConcurrencyType = EConcurrencyType::Default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float Pitch = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float StartTime = 0.0f;
};