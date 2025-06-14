#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_FadeTeleport.generated.h"

class UNiagaraSystem;

UCLASS()
class CR4S_API UAnimNotifyState_FadeTeleport : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Teleport")
    TObjectPtr<UNiagaraSystem> VanishEffect;

    UPROPERTY(EditAnywhere, Category = "Teleport")
    TObjectPtr<UNiagaraSystem> AppearEffect;

    UPROPERTY(EditAnywhere, Category = "Teleport")
    float BackOffset = 200.f;

};
