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
	if (!IsValid(Monster)) return EBTNodeResult::Failed;

	UMonsterSkillComponent* SkillComp = Monster->FindComponentByClass<UMonsterSkillComponent>();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(SkillComp) || !IsValid(BlackboardComp)) return EBTNodeResult::Failed;

	const int32 SkillIndex = BlackboardComp->GetValueAsInt(SkillIndexKey.SelectedKeyName);

	if (!SkillComp->IsSkillReady(SkillIndex))
	{
		CR4S_Log(LogMonster, Warning, TEXT("[%s] Skill %d is not ready."), *MyHeader, SkillIndex);
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsBool(FAIKeys::bIsPlayingAttackMontage, true);
	
	SkillComp->UseSkill(SkillIndex);

	UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>();
	if (!IsValid(AnimComp))
	{
		CR4S_Log(LogMonster, Warning, TEXT("[%s] No montage is playing after UseSkill"), *GetClass()->GetName());
		return EBTNodeResult::Failed;
	}

	AnimComp->OnMontageEndedNotify.RemoveAll(this);
	AnimComp->OnMontageEndedNotify.AddDynamic(this, &UBTTask_PlayAttackMontage::OnAnimMontageEnded);

	CachedOwnerComp = &OwnerComp;

	CR4S_Log(LogMonster, Log, TEXT("[%s] Used SkillID: %d"), *MyHeader, SkillIndex);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_PlayAttackMontage::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!IsValid(CachedOwnerComp)) return EBTNodeResult::Failed;

	AAIController* AIC = CachedOwnerComp->GetAIOwner();
	if (!IsValid(AIC)) return EBTNodeResult::Failed;
		
	APawn* Pawn = AIC->GetPawn();
	if (!IsValid(Pawn)) return EBTNodeResult::Failed;

	if (UMonsterAnimComponent* AnimComp = Pawn->FindComponentByClass<UMonsterAnimComponent>())
	{
		AnimComp->OnMontageEndedNotify.RemoveAll(this);
	}

	if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FAIKeys::bIsPlayingAttackMontage, false);
	}
	
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Aborted;
}

void UBTTask_PlayAttackMontage::OnAnimMontageEnded(UAnimMontage* Montage)
{
	if (!IsValid(CachedOwnerComp)) return;

	AAIController* AIC = CachedOwnerComp->GetAIOwner();
	if (!IsValid(AIC)) return;
		
	APawn* Pawn = AIC->GetPawn();
	if (!IsValid(Pawn)) return;
	
	if (UMonsterAnimComponent* AnimComp = Pawn->FindComponentByClass<UMonsterAnimComponent>())
	{
		AnimComp->OnMontageEndedNotify.RemoveAll(this);
	}
	
	CachedOwnerComp->GetBlackboardComponent()->SetValueAsBool(FAIKeys::bIsPlayingAttackMontage, false);
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	CachedOwnerComp = nullptr;
}
