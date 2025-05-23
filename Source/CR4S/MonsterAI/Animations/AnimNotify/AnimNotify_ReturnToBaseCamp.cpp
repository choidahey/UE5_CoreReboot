#include "MonsterAI/Animations/AnimNotify/AnimNotify_ReturnToBaseCamp.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "AIController.h"
#include "MonsterAI/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Components/MonsterAttributeComponent.h"
#include "MonsterAI/Components/MonsterStateComponent.h"

void UAnimNotify_ReturnToBaseCamp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	ABaseMonster* Monster = Cast<ABaseMonster>(Owner);
	if (!Monster) return;

	AAIController* AIC = Cast<AAIController>(Monster->GetController());
	if (!AIC || !AIC->GetBlackboardComponent())	return;

	// Return
	const FVector TargetLocation = AIC->GetBlackboardComponent()->GetValueAsVector(FRegionBossAIKeys::BaseCampLocation) + FVector(0.f, 0.f, 100.f);
	Monster->SetActorLocation(TargetLocation);

	if (UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>())
	{
		StateComp->SetState(EMonsterState::Idle);	
	}

	if (UMonsterAttributeComponent* AttrComp = Monster->FindComponentByClass<UMonsterAttributeComponent>())
	{
		AttrComp->RecoverToFull();
	}
}
