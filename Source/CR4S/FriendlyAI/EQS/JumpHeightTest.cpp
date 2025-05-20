#include "JumpHeightTest.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AIController.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "GameFramework/Pawn.h"
#include "../Controller/HelperBotAIController.h"
#include "GameFramework/Actor.h"

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
	UObject* Querier = QueryInstance.Owner.Get();
	if (!Querier)
	{
		return;
	}
	
	float MaxJumpHeightValue = MaxJumpHeight;

	FVector QuerierLocation = FVector::ZeroVector;

	if (const AAIController* AICon = Cast<AAIController>(Querier))
	{
		if (const APawn* Pawn = AICon->GetPawn())
		{
			QuerierLocation = Pawn->GetActorLocation();
		}
	}

	for (FEnvQueryInstance::FItemIterator It(this, QueryInstance); It; ++It)
	{
		FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		float HeightDifference = FMath::Abs(ItemLocation.Z - QuerierLocation.Z);
		bool bPass = HeightDifference <= MaxJumpHeightValue;

		It.SetScore(TestPurpose, FilterType, bPass ? 1.f : 0.f, 0.f, MaxJumpHeightValue);
	}
}
