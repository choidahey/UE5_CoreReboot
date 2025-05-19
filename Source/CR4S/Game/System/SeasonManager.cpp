#include "Game/System/SeasonManager.h"
#include "Game/System/SeasonType.h"

bool USeasonManager::ShouldCreateSubsystem(UObject* Outer) const
{
	UWorld* World = Cast<UWorld>(Outer);
	if (World && World->GetName() == TEXT("SurvivalLevel"))
	{
		UE_LOG(LogTemp, Warning, TEXT("SeasonManager: ShouldCreateSubsystem called for SurvivalLevel"));
		return true;  // Creates this subsystem only in the SurvivalLevel world
	}
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: ShouldCreateSubsystem called for other world"));
	return false;
}

void USeasonManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Initialize called"));
	
	LoadSeason();
}

void USeasonManager::LoadSeason()
{
	// if Saved Data Exists, Load Season Data
	// else Set Default Season
	SetCurrentSeason(ESeasonType::BountifulSeason);
}

void USeasonManager::ChangeToNextSeason()
{
	int32 CurrentSeasonInt = static_cast<int32>(CurrentSeason);
	int32 NextSeasonInt = (CurrentSeasonInt + 1) % (static_cast<int32>(ESeasonType::FrostSeason) + 1);

	ESeasonType NextSeason = static_cast<ESeasonType>(NextSeasonInt);
	if (NextSeason == ESeasonType::None)
	{
		NextSeason = ESeasonType::BountifulSeason; // Loop back to the first season
	}

	SetCurrentSeason(NextSeason);
}


void USeasonManager::SetCurrentSeason(ESeasonType NewSeason)
{
	CurrentSeason = NewSeason;
	UE_LOG(LogTemp, Warning, TEXT("Current Season changed to: %s"), *UEnum::GetValueAsString(NewSeason));

	HandleSeasonChange(NewSeason);
}

void USeasonManager::HandleSeasonChange(ESeasonType NewSeason)
{
	// Handle the season change logic here

	UE_LOG(LogTemp, Warning, TEXT("Handling season change to: %s"), *UEnum::GetValueAsString(NewSeason));
}

void USeasonManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Deinitialize called"));
}