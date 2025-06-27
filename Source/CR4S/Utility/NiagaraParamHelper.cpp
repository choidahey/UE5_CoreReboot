#include "Utility/NiagaraParamHelper.h"

void UNiagaraParamUtils::ApplyParams(UNiagaraComponent* NiagaraComp, const FNiagaraParamSet& Params)
{
	if (!NiagaraComp) return;

	for (const auto& Param : Params.FloatParams)
	{
		NiagaraComp->SetVariableFloat(Param.ParamName, Param.Value);
	}

	for (const auto& Param : Params.VectorParams)
	{
		NiagaraComp->SetVariableVec3(Param.ParamName, Param.Value);
	}

	for (const auto& Param : Params.ColorParams)
	{
		NiagaraComp->SetVariableLinearColor(Param.ParamName, Param.Value);
	}
}
