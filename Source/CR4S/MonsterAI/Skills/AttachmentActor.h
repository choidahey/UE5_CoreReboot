#pragma once

#include "CoreMinimal.h"
#include "MonsterAI/Skills/BaseSkillActor.h"
#include "AttachmentActor.generated.h"

UCLASS()
class CR4S_API AAttachmentActor : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:
	AAttachmentActor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const&, AController*, AActor*) override;

private:
	void DeactivateBuffAndDestroy();

protected:
	UPROPERTY(EditAnywhere, Category = "Attachment")
	float Duration = 5.f;

	UPROPERTY(EditAnywhere, Category = "Attachment")
	bool bIsDestructible = false;

	UPROPERTY(EditAnywhere, Category = "Attachment", meta = (EditCondition = "bCanBeDamaged"))
	float MaxHealth = 0.f;

	UPROPERTY()
	float CurrentHealth = 0.f;

	UPROPERTY()
	float ElapsedTime = 0.f;

};
