#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraParamHelper.generated.h"

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

USTRUCT(BlueprintType)
struct FNiagaraColorParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName ParamName;

	UPROPERTY(EditAnywhere)
	FLinearColor Value;
};

USTRUCT(BlueprintType)
struct FNiagaraParamSet
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TArray<FNiagaraFloatParam> FloatParams;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TArray<FNiagaraVectorParam> VectorParams;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TArray<FNiagaraColorParam> ColorParams;
};

UCLASS()
class CR4S_API UNiagaraParamUtils : public UObject
{
	GENERATED_BODY()

public:
	static void ApplyParams(UNiagaraComponent* NiagaraComp, const FNiagaraParamSet& Params);

};
