#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "CR4S/Utility/NiagaraParamHelper.h"
#include "AnimNotify_PlayNiagaraCustom.generated.h"

UCLASS()
class CR4S_API UAnimNotify_PlayNiagaraCustom : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	bool bAttachToMesh = true;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	bool bSnapToGround = false;

	UPROPERTY(EditAnywhere, Category = "Niagara|Parameters")
	FNiagaraParamSet NotifyEffectParams;

	UPROPERTY(EditAnywhere, Category = "Niagara|Ground")
	float GroundTraceDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Niagara|Ground")
	float GroundOffsetZ = 10.f;

};
