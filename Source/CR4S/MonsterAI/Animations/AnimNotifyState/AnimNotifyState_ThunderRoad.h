#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ThunderRoad.generated.h"

UCLASS()
class CR4S_API UAnimNotifyState_ThunderRoad : public UAnimNotifyState
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
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	TSubclassOf<AActor> SpawnActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	FName AttachSocketName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	bool bIsAttachInSocket = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	float BossMoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Sill")
	FVector WallExtent = FVector(50.f, 200.f, 400.f);

private:
	UPROPERTY()
	AActor* SpawnedActor = nullptr;
	
	UPROPERTY()
	FVector TargetActorLocation = FVector::ZeroVector;
	
	UPROPERTY()
	FRotator InitialActorRotation = FRotator::ZeroRotator;

	UPROPERTY()
	FVector InitialDashDirection = FVector::ZeroVector;

	float TraceHeight = 2000.f;

	FString MyHeader = TEXT("ThunderRoad_Notify");
};
