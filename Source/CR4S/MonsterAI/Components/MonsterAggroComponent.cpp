#include "MonsterAggroComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


UMonsterAggroComponent::UMonsterAggroComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
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
	Info.LastDamageTime = GetWorld()->GetTimeSeconds();
	
	if (Info.LastDistance <= 0.f)
	{
		Info.LastDistance = FVector::Dist(Instigator->GetActorLocation(), GetOwner()->GetActorLocation());
	}
}

void UMonsterAggroComponent::DecayAggro(float DeltaTime)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	
	for (auto It = AggroTable.CreateIterator(); It; ++It)
	{
		FAggroInfo& Info = It->Value;
		
		// 마지막 데미지 후 일정 시간이 지나면 어그로 감소 시작
		if (CurrentTime - Info.LastDamageTime > AggroDecayDelay)
		{
			Info.RawDamageAggro = FMath::Max(0.f, Info.RawDamageAggro - AggroDecayRate * DeltaTime);
			
			// 어그로가 임계값 이하로 떨어지면 제거
			if (Info.RawDamageAggro < MinAggroThreshold)
			{
				// 현재 타겟이 제거되는 경우 null로 설정
				if (It->Key.Get() == CurrentTarget.Get())
				{
					CurrentTarget = nullptr;
				}
				It.RemoveCurrent();
			}
		}
	}
}

void UMonsterAggroComponent::RecalculateAggro()
{
	AActor* Best = nullptr;
	float BestScore = -FLT_MAX;

	for (auto& Pair : AggroTable)
	{
		AActor* Target = Pair.Key.Get();
		if (!IsValid(Target)) continue;

		float Raw = Pair.Value.RawDamageAggro;
		float Dist = Pair.Value.LastDistance;
		
		if (Raw < MinAggroThreshold) continue;
		
		float SafeDist = FMath::Max(Dist, 0.01f);
		float Weight = 1.f / FMath::Pow(SafeDist, DistanceExponent);
		float Score = Raw * Weight;
		
		if (Target == CurrentTarget.Get())
		{
			Score *= (1.f + SwitchHysteresis);
		}
		
		if (Score > BestScore)
		{
			Best = Target;
			BestScore = Score;
		}
	}
	//
	// if (BestScore >= MinAggroThreshold)
	// {
	// 	CurrentTarget = Best;
	// }
	// else
	// {
	// 	CurrentTarget = nullptr;
	// }

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
			if (Target == CurrentTarget.Get())
			{
				CurrentTarget = nullptr;
			}
			
			It.RemoveCurrent();
		}
		else
		{
			It->Value.LastDistance = FVector::Dist(Target->GetActorLocation(), GetOwner()->GetActorLocation());
		}
	}

	DecayAggro(DeltaTime);
	RecalculateAggro();
}

bool UMonsterAggroComponent::HasAggroFor(AActor* Target) const
{
	if (!Target) return false;
	
	const FAggroInfo* Info = AggroTable.Find(Target);
	return Info && Info->RawDamageAggro >= MinAggroThreshold;
}

bool UMonsterAggroComponent::HasAnyAggro() const
{
	for (const auto& Pair : AggroTable)
	{
		if (Pair.Value.RawDamageAggro >= MinAggroThreshold)
		{
			return true;
		}
	}
	return false;
}

void UMonsterAggroComponent::CleanupExpiredAggro()
{
	for (auto It = AggroTable.CreateIterator(); It; ++It)
	{
		if (It->Value.RawDamageAggro < MinAggroThreshold)
		{
			if (It->Key.Get() == CurrentTarget.Get())
			{
				CurrentTarget = nullptr;
			}
			It.RemoveCurrent();
		}
	}
}

float UMonsterAggroComponent::GetAggroValueFor(AActor* Target) const
{
	if (!Target) return 0.f;
	
	const FAggroInfo* Info = AggroTable.Find(Target);
	return Info ? Info->RawDamageAggro : 0.f;
}