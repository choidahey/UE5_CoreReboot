#include "Game/System/SeasonManager.h"
#include "Game/System/SeasonType.h"
#include "Game/System/EnvironmentManager.h"
#include "MonsterAI/Data/SeasonBossDataAsset.h"
#include "MonsterAI/BossMonster/Season/SeasonBossMonster.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

bool USeasonManager::ShouldCreateSubsystem(UObject* Outer) const
{
	UWorld* World = Cast<UWorld>(Outer);
	if (World && World->GetName() == TEXT("SurvivalLevel_1"))
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

	//Load Season Boss DataAsset
	const FString AssetPath = TEXT("/Game/CR4S/_Data/Monster/Season/DA_SeasonBoss.DA_SeasonBoss");
	USeasonBossDataAsset* LoadedAsset = Cast<USeasonBossDataAsset>(
		StaticLoadObject(USeasonBossDataAsset::StaticClass(), nullptr, *AssetPath));

	if (LoadedAsset)
	{
		SeasonBossData = LoadedAsset;
		UE_LOG(LogTemp, Log, TEXT("SeasonManager: Successfully loaded SeasonBossDataAsset"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SeasonManager: Failed to load SeasonBossDataAsset from path: %s"), *AssetPath);
	}
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


void USeasonManager::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Deinitialize called"));

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
	else if (CurrentSeasonDay == SeasonLength)
	{
		SpawnSeasonBoss();
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

TSubclassOf<ASeasonBossMonster> USeasonManager::GetSeasonBossClass(ESeasonType Season) const
{
	if (!SeasonBossData) return nullptr;

	for (const FSeasonBossEntry& Entry : SeasonBossData->Bosses)
	{
		if (Entry.SeasonName == Season)
		{
			return Entry.SeasonBossClass;
		}
	}
	return nullptr;
}

void USeasonManager::SpawnSeasonBoss()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TSubclassOf<ASeasonBossMonster> BossClass = GetSeasonBossClass(CurrentSeason);
	if (!BossClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SeasonManager: No boss assigned for season %s"), *UEnum::GetValueAsString(CurrentSeason));
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	if (!PlayerPawn) return;

	const FVector PlayerLocation = PlayerPawn->GetActorLocation();

	const float MinRadius = 5000.f;
	const float MaxRadius = 6000.f;
	const int32 MaxAttempts = 20;

	FVector FinalSpawnLocation = PlayerLocation + FVector(2500.f, 2500.f, 1000.f);
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

	if (NavSys)
	{
		for (int32 i = 0; i < MaxAttempts; ++i)
		{
			const float RandAngle = FMath::RandRange(0.f, 360.f);
			const float RandRadius = FMath::RandRange(MinRadius, MaxRadius);
			const FVector Offset = FVector(FMath::Cos(FMath::DegreesToRadians(RandAngle)), FMath::Sin(FMath::DegreesToRadians(RandAngle)), 0.f) * RandRadius;

			const FVector TestLocation = PlayerLocation + Offset;

			FNavLocation OutNavLocation;
			if (NavSys->ProjectPointToNavigation(TestLocation, OutNavLocation))
			{
				FinalSpawnLocation = OutNavLocation.Location;
				UE_LOG(LogTemp, Log, TEXT("SeasonManager: Found valid boss spawn location after %d attempts"), i + 1);
				break;
			}
		}
	}

	if (SpawnedBoss && IsValid(SpawnedBoss))
	{
		SpawnedBoss->Destroy();
	}

	SpawnedBoss = World->SpawnActor<ASeasonBossMonster>(BossClass, FinalSpawnLocation, FRotator::ZeroRotator);
	if (SpawnedBoss)
	{
		UE_LOG(LogTemp, Warning, TEXT("SeasonManager: Spawned boss for season %s"), *UEnum::GetValueAsString(CurrentSeason));
	}
}


void USeasonManager::HandleSeasonChange(ESeasonType NewSeason)
{


	UE_LOG(LogTemp, Warning, TEXT("Handling season change to: %s"), *UEnum::GetValueAsString(NewSeason));
}