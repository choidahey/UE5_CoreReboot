#include "MonsterAI/Animations/AnimNotify/AnimNotify_Teleport.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "AIController.h"
#include "MonsterAI/BaseMonster.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "MonsterAI/Components/MonsterStateComponent.h"

void UAnimNotify_Teleport::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	ABaseMonster* Monster = Cast<ABaseMonster>(Owner);
	if (!Monster)
	{
		return;
	}

	AAIController* AIC = Cast<AAIController>(Monster->GetController());
	if (!AIC || !AIC->GetBlackboardComponent())
	{
		return;
	}

	// Teleport
	const FVector TargetLocation = AIC->GetBlackboardComponent()->GetValueAsVector(FRegionBossAIKeys::BaseCampLocation);
	Monster->SetActorLocation(TargetLocation);

	if (LandFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(Monster->GetWorld(), LandFX, TargetLocation);
	}

	if (UMonsterStateComponent* StateComp = Monster->FindComponentByClass<UMonsterStateComponent>())
	{
		StateComp->SetState(EMonsterState::Idle);	
	}
}
