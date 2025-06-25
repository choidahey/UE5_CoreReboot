#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_DashSkill.generated.h"

class UCapsuleComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class CR4S_API UAnimNotifyState_DashSkill : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
						 UAnimSequenceBase* Animation,
						 float TotalDuration) override;
	
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp,
							UAnimSequenceBase* Animation,
							float FrameDeltaTime) override;
	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp,
						   UAnimSequenceBase* Animation) override;
	
	virtual void BeginDestroy() override;

protected:
	void CleanupSpawnedActors();

#pragma region Use Spawn Actor
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill")
	bool bUseActorSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill", meta=(EditCondition="bUseActorSpawn"))
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill", meta=(EditCondition="bUseActorSpawn"))
	FName ActorAttachSocketName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill", meta=(EditCondition="bUseActorSpawn"))
	bool bIsAttachInSocket = false;

#pragma endregion Use Spawn Actor

#pragma region Use Niagara Spawn
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill")
	bool bUseNiagaraEffect = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill", meta=(EditCondition="bUseNiagaraEffect"))
	UNiagaraSystem* NiagaraEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill", meta=(EditCondition="bUseNiagaraEffect"))
	FName NiagaraAttachSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill", meta=(EditCondition="bUseNiagaraEffect"))
	FVector NiagaraLocationOffset = FVector::ZeroVector;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill", meta=(EditCondition="bUseNiagaraEffect"))
	FRotator NiagaraRotationOffset = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill", meta=(EditCondition="bUseNiagaraEffect"))
	FVector NiagaraScale = FVector(1.f, 1.f, 1.f);
	
#pragma endregion Use Niagara Spawn

#pragma region Dash Option
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill")
	bool bOppositeDirection = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	float BossMoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Sill")
	float DashDistance = 0.f;

#pragma endregion Dash Option

private:
	TWeakObjectPtr<AActor> SpawnedActor;
	TWeakObjectPtr<UNiagaraComponent> SpawnedNiagaraComp;
	
	FVector TargetActorLocation = FVector::ZeroVector;
	FVector StartingLocation = FVector::ZeroVector;
	FRotator InitialActorRotation = FRotator::ZeroRotator;
	FVector InitialDashDirection = FVector::ZeroVector;
	float TraceHeight = 2000.f;

	FString MyHeader = TEXT("ThunderRoad_Notify");
};
