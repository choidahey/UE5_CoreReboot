#include "Game/System/SeasonManager.h"
#include "Game/System/SeasonType.h"
#include "Game/System/EnvironmentManager.h"
#include "Kismet/GameplayStatics.h"

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

	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &USeasonManager::OnPostWorldInit);
}

void USeasonManager::OnPostWorldInit(UWorld* World, const UWorld::InitializationValues IVS)
{
	EnvironmentManager = Cast<AEnvironmentManager>(UGameplayStatics::GetActorOfClass(World, AEnvironmentManager::StaticClass()));
	if (!EnvironmentManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SeasonManager: EnvironmentManager not found"));
	}
}

void USeasonManager::LoadSeason()
{
	// if Saved Data Exists, Load Season Data
	// else Set Default Season
	UE_LOG(LogTemp, Warning, TEXT("Loading Season"));
	SetCurrentSeason(ESeasonType::BountifulSeason);
}

void USeasonManager::HandleDayChange()
{
	if (!EnvironmentManager) return;
		
	float Progress = static_cast<float>(CurrentSeasonDay) / static_cast<float>(SeasonLength);
	CurrentDayNightRatio = FMath::Lerp(StartDayNightRatio, TargetDayNightRatio, Progress);

	EnvironmentManager->SetDayNightRatio(CurrentDayNightRatio);

	CurrentSeasonDay++;

	if (CurrentSeasonDay > SeasonLength)
	{
		ChangeToNextSeason();
		CurrentSeasonDay = 1;
		StartDayNightRatio = CurrentDayNightRatio;
		TargetDayNightRatio = GetTargetDayRatioForSeason(CurrentSeason);
	}

	UE_LOG(LogTemp, Log, TEXT("AdjustDayNightLength: Day %d / %d, Ratio %.3f"), CurrentSeasonDay, SeasonLength, CurrentDayNightRatio);
}



void USeasonManager::ChangeToNextSeason()
{
	int32 CurrentSeasonInt = static_cast<int32>(CurrentSeason);
	int32 NextSeasonInt = (CurrentSeasonInt + 1) % (static_cast<int32>(ESeasonType::DrySeason) + 1);

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

	EnvironmentManager->SetWeatherBySeason(NewSeason);
}

void USeasonManager::HandleSeasonChange(ESeasonType NewSeason)
{


	UE_LOG(LogTemp, Warning, TEXT("Handling season change to: %s"), *UEnum::GetValueAsString(NewSeason));
}

float USeasonManager::GetTargetDayRatioForSeason(ESeasonType Season) const
{
	switch (Season)
	{
	case ESeasonType::BountifulSeason: return 0.3f;
	case ESeasonType::FrostSeason: return 0.4f;
	case ESeasonType::RainySeason: return 0.7f;
	case ESeasonType::DrySeason: return 0.5f;
	default: return 0.5f;
	}
}

void USeasonManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Deinitialize called"));
}