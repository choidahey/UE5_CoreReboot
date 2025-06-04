#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_AccumulateDamage.h"
#include "MonsterAI/BaseMonster.h"
#include "MonsterAI/Components/MonsterAnimComponent.h"
#include "Kismet/GameplayStatics.h"

void UAnimNotifyState_AccumulateDamage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner()))
	{
		Monster->ClearAccumulatedDamage();
		UE_LOG(LogTemp, Log, TEXT("[AccumulateDamage] Begin - Reset Accumulated Damage."));
	}
}

void UAnimNotifyState_AccumulateDamage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner()))
	{
		float Accumulated = Monster->GetAccumulatedDamage();
		UE_LOG(LogTemp, Log, TEXT("[AccumulateDamage] End - Accumulated Damage : %f."), Accumulated);

		if (Accumulated >= DamageThreshold)
		{
			if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
			{
				UE_LOG(LogTemp, Log, TEXT("[AccumulateDamage] End - Playing Montage."));

				AnimInst->Montage_Stop(0.2f);
				if (UMonsterAnimComponent* AnimComp = Monster->FindComponentByClass<UMonsterAnimComponent>())
				{
					AnimComp->PlayHitReactMontage();
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[AccumulateDamage] End - Attack Resumed."));
		}
	}
}
