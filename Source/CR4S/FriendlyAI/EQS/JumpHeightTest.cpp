#include "JumpHeightTest.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UJumpHeightTest::UJumpHeightTest()
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Point::StaticClass();
	MaxJumpHeight = 300.f;
	TestPurpose = EEnvTestPurpose::Filter;
	FilterType = EEnvTestFilterType::Maximum;
}

void UJumpHeightTest::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QuerierObj = QueryInstance.Owner.Get();
	AAIController* AICon = Cast<AAIController>(QuerierObj);
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

	// 중력과 초기 속도 계산
	const float Gravity = FMath::Abs(World->GetGravityZ());
	const float InitialVz = FMath::Sqrt(2.f * Gravity * MaxJumpHeight);
	const float FlightTime = (2.f * InitialVz) / Gravity;

	// 최대 수평 이동 거리 계산
	float MaxHorizontalDistance = 0.f;
	if (const UCharacterMovementComponent* MoveComp = Pawn->FindComponentByClass<UCharacterMovementComponent>())
	{
		MaxHorizontalDistance = MoveComp->MaxWalkSpeed * FlightTime;
	}

	const FVector StartLoc = Pawn->GetActorLocation();

	for (FEnvQueryInstance::FItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLoc = GetItemLocation(QueryInstance, It.GetIndex());
		const float DeltaZ = FMath::Abs(ItemLoc.Z - StartLoc.Z);
		const float DeltaXY = FVector2D(ItemLoc - StartLoc).Size();
		const bool bPass = (DeltaZ <= MaxJumpHeight) && (DeltaXY <= MaxHorizontalDistance);
		
		if (bPass)
		{
			DrawDebugCapsule(World, ItemLoc, 10.f, 5.f, FQuat::Identity, FColor::Green, false, 1.f);
		}
		else
		{
			DrawDebugCapsule(World, ItemLoc, 10.f, 5.f, FQuat::Identity, FColor::Red, false, 1.f);
		}

		It.SetScore(TestPurpose, FilterType, bPass ? 1.f : 0.f, 0.f, MaxJumpHeight);
	}
}