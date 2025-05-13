#pragma once

UENUM(BlueprintType)
enum class ESeasonType : uint8
{
	None				UMETA(DisplayName = "None"),
	BountifulSeason		UMETA(DisplayName = "Bountiful"),
	RainySeason			UMETA(DisplayName = "Rainy"),
	DrySeason			UMETA(DisplayName = "Dry"),
	FrostSeason			UMETA(DisplayName = "Frost")
};