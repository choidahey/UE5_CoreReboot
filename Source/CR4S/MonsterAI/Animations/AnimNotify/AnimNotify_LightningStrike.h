#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_LightningStrike.generated.h"

class UNiagaraSystem;
class ALightningStrikeActor;

UCLASS()
class CR4S_API UAnimNotify_LightningStrike : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere, Category = "Lightning")
	TSubclassOf<ALightningStrikeActor> LightningStrikeClass;

	UPROPERTY(EditAnywhere, Category = "Lightning")
	TObjectPtr<UNiagaraSystem> LightningEffect;

	UPROPERTY(EditAnywhere, Category = "Lightning")
	float SpawnRadius = 800.f;

};
