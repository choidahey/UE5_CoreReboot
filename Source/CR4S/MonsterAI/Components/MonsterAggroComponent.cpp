#include "MonsterAggroComponent.h"

#include "BehaviorTree/BlackboardComponent.h"


UMonsterAggroComponent::UMonsterAggroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMonsterAggroComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMonsterAggroComponent::AddDamageAggro(AActor* Instigator, float Damage)
{
	if (!Instigator) return;
	FAggroInfo& Info = AggroTable.FindOrAdd(Instigator);
	Info.RawDamageAggro += Damage * DamageFactor;
	
	if (Info.LastDistance <= 0.f)
	{
		Info.LastDistance = FVector::Dist(Instigator->GetActorLocation(), GetOwner()->GetActorLocation());
	}
}

void UMonsterAggroComponent::RecalculateAggro()
{
	AActor* Best      = nullptr;
	float   BestScore = -FLT_MAX;

	for (auto& Pair : AggroTable)
	{
		AActor* Target = Pair.Key.Get();
		if (!IsValid(Target)) continue;

		float Raw  = Pair.Value.RawDamageAggro;
		float Dist = Pair.Value.LastDistance;
		
		float SafeDist = FMath::Max(Dist, 0.01f);
		float Weight   = 1.f / FMath::Pow(SafeDist, DistanceExponent);
		float Score = Raw * Weight;
		
		if (Target == CurrentTarget.Get())
		{
			Best = Target;
			BestScore = Score;
		}
		else if (Score > BestScore * (1.f + SwitchHysteresis))
		{
			Best = Target;
			BestScore = Score;
		}
	}

	CurrentTarget = Best;
}


void UMonsterAggroComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto It = AggroTable.CreateIterator(); It; ++It)
	{
		AActor* Target = It->Key.Get();
		if (!IsValid(Target))
		{
			It.RemoveCurrent();
		}
		else
		{
			It->Value.LastDistance = FVector::Dist(
				Target->GetActorLocation(),
				GetOwner()->GetActorLocation());
		}
	}
	
	RecalculateAggro();

}

void UMonsterAggroComponent::ApplyToBlackboard(class UBlackboardComponent* BB, const struct FBlackboardKeySelector& KeySelector) const
{
	if (BB)
	{
		BB->SetValueAsObject(KeySelector.SelectedKeyName, CurrentTarget.Get());
	}
}

