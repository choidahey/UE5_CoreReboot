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

	// 나중에 추가될 수 있는 설정들을 위한 예시
	// UPROPERTY(VisibleAnywhere, Category = "Graphics")
	// int32 ScreenResolutionIndex;
};
