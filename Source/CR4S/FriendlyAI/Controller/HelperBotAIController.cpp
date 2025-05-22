#include "HelperBotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../EQS/JumpHeightTest.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "../Component/GroundMovementComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

AHelperBotAIController::AHelperBotAIController()
{
	bAttachToPawn = true;
}

void AHelperBotAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBotLogic();

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		Blackboard->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
	}
}

void AHelperBotAIController::RunBotLogic()
{
	if (BehaviorTreeAsset && BlackboardAsset)
	{
		UBlackboardComponent* BlackboardComp = nullptr;
		UseBlackboard(BlackboardAsset, BlackboardComp);
		RunBehaviorTree(BehaviorTreeAsset);
		Blackboard = BlackboardComp;
	}
}

void AHelperBotAIController::SetBotState(EHelperBotState NewState)
{
	if (!Blackboard) return;

	Blackboard->SetValueAsEnum(StateKeyName, static_cast<uint8>(NewState));

	if (NewState == EHelperBotState::Following)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			Blackboard->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
		}
	}
}

void AHelperBotAIController::RunEQSQuery()
{
	if (!BehaviorTreeAsset)
		return;

	UEnvQuery* EQSQuery = LoadObject<UEnvQuery>(nullptr, TEXT("/Game/AI/EQS_HelperBot_JumpTest.EQS_HelperBot_JumpTest"));
	if (!EQSQuery)
		return;

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this,
		EQSQuery,
		this,
		EEnvQueryRunMode::SingleResult,
		nullptr);

	if (QueryInstance)
	{
		QueryInstance->SetNamedParam(FName("MaxJumpHeight"), JumpHeight);
		
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &AHelperBotAIController::OnEQSQueryFinished);
	}
}

void AHelperBotAIController::OnEQSQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (!QueryInstance || QueryStatus != EEnvQueryStatus::Success)
		return;

	FVector MoveLocation = QueryInstance->GetResultsAsLocations()[0];
	if (Blackboard)
	{
		Blackboard->SetValueAsVector(TEXT("MoveToLocation"), MoveLocation);
	}
}