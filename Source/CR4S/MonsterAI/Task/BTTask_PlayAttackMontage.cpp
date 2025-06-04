#include "MonsterAI/Task/BTTask_PlayAttackMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/MonsterAIHelper.h"

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

	const int32 SkillIndex = BlackboardComp->GetValueAsInt(SkillIndexKey.SelectedKeyName);

	if (!SkillComp->IsSkillReady(SkillIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[UBTTask_PlayAttackMontage] Skill %d is not ready."), SkillIndex);
		return EBTNodeResult::Failed;
	}

	// SkillComp->OnSkillMontageEnded.AddUniqueDynamic(this, &UBTTask_PlayAttackMontage::OnMontageEnded);
	CachedOwnerComp = &OwnerComp;
	CachedSkillComp = SkillComp;

	SkillComp->UseSkill(SkillIndex);

	UE_LOG(LogTemp, Log, TEXT("[UBTTask_PlayAttackMontage] Used SkillID: %d"), SkillIndex);
	return EBTNodeResult::InProgress;
}

void UBTTask_PlayAttackMontage::OnMontageEnded(bool bInterrupted)
{
	if (!CachedSkillComp || !CachedOwnerComp) return;

	// CachedSkillComp->OnSkillMontageEnded.RemoveDynamic(this, &UBTTask_PlayAttackMontage::OnMontageEnded);
	const EBTNodeResult::Type Result = bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	FinishLatentTask(*CachedOwnerComp, Result);
}

void UBTTask_PlayAttackMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	//if (CachedSkillComp)
	//{
	//	CachedSkillComp->OnSkillMontageEnded.RemoveDynamic(this, &UBTTask_PlayAttackMontage::OnMontageEnded);
	//}
	CachedSkillComp = nullptr;
	CachedOwnerComp = nullptr;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
