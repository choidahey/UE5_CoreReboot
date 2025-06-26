#include "MonsterAI/Animations/AnimNotify/AnimNotify_PlayNiagaraCustom.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CR4S.h"

void UAnimNotify_PlayNiagaraCustom::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!NiagaraSystem || !MeshComp) return;

	UWorld* World = MeshComp->GetWorld();
	if (!World) return;

	FVector SpawnLocation = MeshComp->GetComponentLocation();
	FRotator SpawnRotation = MeshComp->GetComponentRotation();

	// Socket/Offset Àû¿ë
	if (AttachSocketName != NAME_None)
	{
		SpawnLocation = MeshComp->GetSocketLocation(AttachSocketName);
		SpawnRotation = MeshComp->GetSocketRotation(AttachSocketName);
	}

	SpawnLocation += SpawnRotation.RotateVector(LocationOffset);
	SpawnRotation += RotationOffset;

	// Snap to ground
	if (bSnapToGround)
	{
		FHitResult Hit;
		FVector Start = SpawnLocation + FVector(0, 0, 1000);
		FVector End = SpawnLocation - FVector(0, 0, GroundTraceDistance);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(MeshComp->GetOwner());

		if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params))
		{
			SpawnLocation = Hit.ImpactPoint + FVector(0, 0, GroundOffsetZ);
		}
	}

	// Spawn Niagara Effects
	UNiagaraComponent* NiagaraComp = nullptr;
	if (bAttachToMesh)
	{
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			NiagaraSystem,
			MeshComp,
			AttachSocketName,
			LocationOffset,
			RotationOffset,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}
	else
	{
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			NiagaraSystem,
			SpawnLocation,
			SpawnRotation,
			FVector(1.f),
			true,
			true,
			ENCPoolMethod::None,
			true
		);
	}

	// Adjust User Parameters
	if (NiagaraComp)
	{
		for (const FNiagaraFloatParam& Param : FloatParameters)
		{
			NiagaraComp->SetVariableFloat(Param.ParamName, Param.Value);
		}

		for (const FNiagaraVectorParam& Param : VectorParameters)
		{
			NiagaraComp->SetVariableVec3(Param.ParamName, Param.Value);
		}
	}
}
