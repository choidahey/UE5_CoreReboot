#pragma once

UENUM(BlueprintType)
enum class ESeasonType : uint8
{
	None				UMETA(DisplayName = "None"),
	BountifulSeason		UMETA(DisplayName = "Bountiful"),
	FrostSeason			UMETA(DisplayName = "Frost"),
	RainySeason			UMETA(DisplayName = "Rainy"),
	DrySeason			UMETA(DisplayName = "Dry"),
};