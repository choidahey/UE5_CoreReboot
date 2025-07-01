#pragma once

#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

UCLASS()
class CR4S_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	USettingsSaveGame();

	// Graphics Settings //
	UPROPERTY(VisibleAnywhere, Category = "Graphics")
	int32 WindowModeIndex;
	UPROPERTY(VisibleAnywhere, Category = "Graphics")
	int32 ResolutionIndex;
	UPROPERTY(VisibleAnywhere, Category = "Graphics")
	int32 GraphicsQualityIndex;

	// Audio Volume Settings //
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float MasterVolume;
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float BGMVolume;
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	float SFXVolume;

};
