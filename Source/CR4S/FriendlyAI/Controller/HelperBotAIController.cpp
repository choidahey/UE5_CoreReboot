#include "HelperBotAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AHelperBotAIController::AHelperBotAIController()
{
	bAttachToPawn = true;
}

void AHelperBotAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBotLogic();
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
