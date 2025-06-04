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


	GetTargetDawnDuskTimeForSeason(CurrentSeason, TargetDawnTime, TargetDuskTime);
}

void USeasonManager::LoadSeason()
{
	// if Saved Data Exists, Load Season Data
	// else Set Default Season
	UE_LOG(LogTemp, Warning, TEXT("Loading Season"));
	SetCurrentSeason(ESeasonType::BountifulSeason);

	GetTargetDawnDuskTimeForSeason(CurrentSeason, TargetDawnTime, TargetDuskTime);
}

void USeasonManager::HandleDayChange()
{
	if (!EnvironmentManager) return;

	float Progress = static_cast<float>(CurrentSeasonDay) / static_cast<float>(SeasonLength);
	CurrentDawnTime = FMath::Lerp(StartDawnTime, TargetDawnTime, Progress);
	CurrentDuskTime = FMath::Lerp(StartDuskTime, TargetDuskTime, Progress);

	EnvironmentManager->SetDayNightByTime(CurrentDawnTime, CurrentDuskTime);
	OnDayChanged.Broadcast(CurrentDawnTime, CurrentDuskTime);

	CurrentSeasonDay++;

	if (CurrentSeasonDay > SeasonLength)
	{
		ChangeToNextSeason();
		CurrentSeasonDay = 1;

		StartDawnTime = CurrentDawnTime;
		StartDuskTime = CurrentDuskTime;
	}
}

void USeasonManager::GetTargetDawnDuskTimeForSeason(ESeasonType Season, float& OutDawnTime, float& OutDuskTime)
{
	switch (Season)
	{
	case ESeasonType::BountifulSeason:
		OutDawnTime = 900.0f;
		OutDuskTime = 1500.0f;
		break;
	case ESeasonType::FrostSeason:
		OutDawnTime = 800.0f;
		OutDuskTime = 1600.0f;
		break;
	case ESeasonType::RainySeason:
		OutDawnTime = 500.0f;
		OutDuskTime = 1900.0f;
		break;
	case ESeasonType::DrySeason:
		OutDawnTime = 600.0f;
		OutDuskTime = 1600.0f;
		break;
	default:
		OutDawnTime = 600.0f;
		OutDuskTime = 1800.0f;
		break;
	}
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
	OnSeasonChanged.Broadcast(NextSeason);
}


void USeasonManager::SetCurrentSeason(ESeasonType NewSeason)
{
	CurrentSeason = NewSeason;
	UE_LOG(LogTemp, Warning, TEXT("Current Season changed to: %s"), *UEnum::GetValueAsString(NewSeason));

	EnvironmentManager->SetWeatherBySeason(NewSeason);

	GetTargetDawnDuskTimeForSeason(CurrentSeason, TargetDawnTime, TargetDuskTime);
}

void USeasonManager::HandleSeasonChange(ESeasonType NewSeason)
{


	UE_LOG(LogTemp, Warning, TEXT("Handling season change to: %s"), *UEnum::GetValueAsString(NewSeason));
}

void USeasonManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Deinitialize called"));
}