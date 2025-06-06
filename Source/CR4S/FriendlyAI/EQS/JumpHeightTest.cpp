#include "JumpHeightTest.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

UJumpHeightTest::UJumpHeightTest()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Point::StaticClass();
	MaxJumpHeight = 1000.f;
	// TestPurpose = EEnvTestPurpose::Filter;
	// FilterType = EEnvTestFilterType::Maximum;
	TestPurpose = EEnvTestPurpose::Score;
	FilterType = EEnvTestFilterType::Range;
}

void UJumpHeightTest::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QuerierObj = QueryInstance.Owner.Get();
	ACharacter* Character = Cast<ACharacter>(QuerierObj);
	AAIController* AICon = Character ? Cast<AAIController>(Character->GetController()) : nullptr;
	if (!AICon)
	{
		return;
	}

	UWorld* World = AICon->GetWorld();
	if (!World)
	{
		return;
	}

	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		return;
	}
	
	const float Gravity = FMath::Abs(World->GetGravityZ());
	const float InitialVz = FMath::Sqrt(2.f * Gravity * MaxJumpHeight);
	const float FlightTime = (2.f * InitialVz) / Gravity;
	
	float MaxHorizontalDistance = 0.f;
	if (const UCharacterMovementComponent* MoveComp = Pawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		MaxHorizontalDistance = MoveComp->MaxWalkSpeed * FlightTime;
	}

	const FVector StartLoc = Pawn->GetActorLocation();

	FVector TargetLoc = StartLoc;
	{
		const UBlackboardComponent* BBComp = AICon->GetBlackboardComponent();
		if (BBComp)
		{
			UObject* TargetObj = BBComp->GetValueAsObject(TEXT("TargetActor"));
			if (AActor* TargetActor = Cast<AActor>(TargetObj))
			{
				TargetLoc = TargetActor->GetActorLocation();
			}
		}
	}

	for (FEnvQueryInstance::FItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLoc = GetItemLocation(QueryInstance, It.GetIndex());
		const float DeltaXY = FVector2D(ItemLoc - StartLoc).Size();
		const float DeltaZ = ItemLoc.Z - StartLoc.Z;
		if (MaxJumpHeight <= 0.f)
		{
			bool bDown = (DeltaZ <= 0.f);
			float Score = bDown ? 1.f : 0.f;
			//It.SetScore(TestPurpose, FilterType, Score, 0.f, 1.f);
			It.SetScore(EEnvTestPurpose::Score, EEnvTestFilterType::Range, Score, 0.f, 1.f);
			continue;
		}
		
		bool bDown = (DeltaZ < 0.f);
		bool bPass = bDown || (DeltaZ <= MaxJumpHeight);
		bool bJumpNeeded = (DeltaZ > 0.f) && (DeltaZ <= MaxJumpHeight);
		
		UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
		const ANavigationData* NavData = NavSys ? NavSys->GetDefaultNavDataInstance() : nullptr;
		FPathFindingQuery PathQuery(nullptr, *NavData, ItemLoc, TargetLoc);

		const float MinScore = 0.1f;
		float Score = 0.f;
		if (bPass && NavSys && NavData)
		{
			FPathFindingResult Result = NavSys->FindPathSync(PathQuery);
			float PathLen = Result.Path.IsValid()
				? Result.Path->GetLength()
				: TNumericLimits<float>::Max();

			if (bJumpNeeded)
			{
				Score = FMath::Clamp(1.5f - (PathLen / MaxHorizontalDistance), MinScore, 1.f);
			}
			else
			{
				Score = FMath::Clamp(1.f - (PathLen / MaxHorizontalDistance), MinScore, 1.f);
			}
		}
		//It.SetScore(TestPurpose, FilterType, Score, 0.f, 1.f);
		It.SetScore(EEnvTestPurpose::Score, EEnvTestFilterType::Range, Score, 0.f, 1.f);
	}
}