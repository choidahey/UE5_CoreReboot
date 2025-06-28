#include "Game/SaveGame/SettingsSaveGame.h"

USettingsSaveGame::USettingsSaveGame()
{
	WindowModeIndex = 0;
	ResolutionIndex = 2;
	GraphicsQualityIndex = 1;

	MasterVolume = 1.0f;
	BGMVolume = 1.0f;
	SFXVolume = 1.0f;
}
