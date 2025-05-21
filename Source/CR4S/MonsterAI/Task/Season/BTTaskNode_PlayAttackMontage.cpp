#include "MonsterAI/Task/Season/BTTaskNode_PlayAttackMontage.h"
#include "MonsterAI/Components/MonsterSkillComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "MonsterAI/MonsterAIHelper.h"
#include "MonsterAI/BaseMonster.h"


UBTTaskNode_PlayAttackMontage::UBTTaskNode_PlayAttackMontage()
{
	NodeName = TEXT("SeasonBossPlayAttackMontage");
	bNotifyTick = false;
}

EBTNodeResult::Type UBTTaskNode_PlayAttackMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseMonster* MonsterCharacter = MonsterAIHelper::GetControlledMonster(OwnerComp);
	UMonsterSkillComponent* SkillComp = MonsterCharacter->GetSkillComponent();
	UMonsterStateComponent* StateComp = MonsterCharacter->GetStateComponent();

	USkeletalMeshComponent* SkeletalMeshComp = MonsterCharacter->GetMesh();
	if (!SkeletalMeshComp)
		return EBTNodeResult::Failed;

	const FMonsterSkillData& SkillData = SkillComp->GetCurrentSkillData();
	UAnimMontage* Montage = SkillData.SkillMontage;
	if (!Montage)
		return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = SkeletalMeshComp->GetAnimInstance();
	if (!AnimInstance)
		return EBTNodeResult::Failed;

	
	// 현재 쿨이 돌아있는 스킬 인덱스 가져옴
	// 스킬 조건 체크

	StateComp->SetState(EMonsterState::Attack);
	SkillComp->UseSkill(SkillComp->CurrentSkillIndex);

	return EBTNodeResult::Succeeded;
}

