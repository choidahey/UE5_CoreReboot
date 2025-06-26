#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "AnimNotify_PlayNiagaraCustom.generated.h"

USTRUCT(BlueprintType)
struct FNiagaraFloatParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ParamName;

	UPROPERTY(EditAnywhere)
	float Value;
};

USTRUCT(BlueprintType)
struct FNiagaraVectorParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ParamName;

	UPROPERTY(EditAnywhere)
	FVector Value;
};

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
	TArray<FNiagaraFloatParam> FloatParameters;

	UPROPERTY(EditAnywhere, Category = "Niagara|Parameters")
	TArray<FNiagaraVectorParam> VectorParameters;

	UPROPERTY(EditAnywhere, Category = "Niagara|Ground")
	float GroundTraceDistance = 3000.f;

	UPROPERTY(EditAnywhere, Category = "Niagara|Ground")
	float GroundOffsetZ = 10.f;

};
