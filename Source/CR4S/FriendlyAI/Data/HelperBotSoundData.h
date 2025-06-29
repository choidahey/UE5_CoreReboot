#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "HelperBotSoundData.generated.h"

UCLASS(BlueprintType)
class CR4S_API UHelperBotSoundData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(CallInEditor, Category = "Sound")
	TArray<FString> GetSoundPropertyNames() const;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> DeathSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> RecallSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> WorkLaserSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> InteractSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> InteractButtonHoverSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> CommandResponseSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<USoundBase> SelfRepairUISound;
};
