#include "MonsterAI/Animations/AnimNotifyState/AnimNotifyState_IceRoadSkill.h"
#include "MonsterAI/BaseMonster.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MonsterAI/Data/MonsterAIKeyNames.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"


void UAnimNotifyState_IceRoadSkill::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	ElapsedTime = 0.2f;
	Duration = TotalDuration;

	ABaseMonster* Monster = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!Monster) return;

	AAIController* AIC = Cast<AAIController>(Monster->GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	const bool bForward = BB->GetValueAsBool(FSeasonBossAIKeys::bIsIceRoadForward);

	AActor* Player = UGameplayStatics::GetPlayerCharacter(Monster->GetWorld(), 0);
	if (!Player) return;

	FVector Dir = Player->GetActorLocation() - Monster->GetActorLocation();
	Dir.Z = 0.f;
	Dir = Dir.IsNearlyZero() ? Monster->GetActorForwardVector() : Dir.GetSafeNormal();

	StartLocation = Monster->GetActorLocation();
	const float Dist = bForward ? ForwardDistance : AwayDistance;
	TargetLocation = StartLocation + (bForward ? Dir : -Dir) * Dist;

}

void UAnimNotifyState_IceRoadSkill::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	ElapsedTime += FrameDeltaTime;
	const float Alpha = Duration > 0.f ? FMath::Clamp(ElapsedTime / Duration, 0.f, 1.f) : 1.f;
	
	FVector NewLoc = FMath::Lerp(StartLocation, TargetLocation, Alpha);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		Owner->SetActorLocation(NewLoc);
	}

}

void UAnimNotifyState_IceRoadSkill::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
}
