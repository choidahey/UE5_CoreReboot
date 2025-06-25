#pragma once

#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

UCLASS()
class CR4S_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USettingsSaveGame();

	// Audio Volume Settings
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float MasterVolume;
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float BGMVolume;
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float SFXVolume;

	// UPROPERTY(VisibleAnywhere, Category = "Graphics")
	// int32 ScreenResolutionIndex;
};
