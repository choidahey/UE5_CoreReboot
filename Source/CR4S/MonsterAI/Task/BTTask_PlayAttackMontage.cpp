#include "MonsterAI/Task/BTTask_PlayAttackMontage.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "CR4S.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"

UBTTask_PlayAttackMontage::UBTTask_PlayAttackMontage()
{
	NodeName = TEXT("PlayAttackMontage");
	SkillIndexKey.SelectedKeyName = FAIKeys::SkillIndex;
}

EBTNodeResult::Type UBTTask_PlayAttackMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* Monster = MonsterAIHelper::GetControlledMonster(OwnerComp);
	if (!Monster) return EBTNodeResult::Failed;

	UMonsterSkillComponent* SkillComp = Monster->FindComponentByClass<UMonsterSkillComponent>();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!SkillComp || !BlackboardComp) return EBTNodeResult::Failed;
	
	UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>();
	if (!AnimComp)
	{
		CR4S_Log(LogMonster, Warning, TEXT("[%s] Missing MonsterAnimComponent"), *MyHeader);
		return EBTNodeResult::Failed;
	}

	const int32 SkillIndex = BlackboardComp->GetValueAsInt(SkillIndexKey.SelectedKeyName);

	if (!SkillComp->IsSkillReady(SkillIndex))
	{
		CR4S_Log(LogMonster, Warning, TEXT("[%s] Skill %d is not ready."), *MyHeader, SkillIndex);
		return EBTNodeResult::Failed;
	}

	SkillComp->UseSkill(SkillIndex);

	AnimComp->OnMontageEndedNotify.RemoveAll(this);
	AnimComp->OnMontageEndedNotify.AddDynamic(this, &UBTTask_PlayAttackMontage::OnAnimMontageEnded);

	CachedOwnerComp = &OwnerComp;

	CR4S_Log(LogMonster, Log, TEXT("[%s] Used SkillID: %d"), *MyHeader, SkillIndex);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PlayAttackMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (CachedOwnerComp)
	{
		if (AAIController* AIC = CachedOwnerComp->GetAIOwner())
		{
			if (APawn* Pawn = AIC->GetPawn())
			{
				if (UMonsterAnimComponent* AnimComp = Pawn->FindComponentByClass<UMonsterAnimComponent>())
				{
					AnimComp->OnMontageEndedNotify.RemoveAll(this);
				}
			}
		}
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	// TODO :: 여기 너무 부자연스럽게 끊기는데 자연스럽게 연구하기
	CachedOwnerComp->GetBlackboardComponent()->SetValueAsBool(FAIKeys::bIsPlayingAttackMontage, false);
	return EBTNodeResult::Aborted;
}

void UBTTask_PlayAttackMontage::OnAnimMontageEnded(UAnimMontage* Montage)
{
	if (!CachedOwnerComp) return;
	
	if (AAIController* AIC = CachedOwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			if (UMonsterAnimComponent* AnimComp = Pawn->FindComponentByClass<UMonsterAnimComponent>())
			{
				AnimComp->OnMontageEndedNotify.RemoveAll(this);
			}
		}
	}
	
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp->GetBlackboardComponent()->SetValueAsBool(FAIKeys::bIsPlayingAttackMontage, false);
	CachedOwnerComp = nullptr;
}
