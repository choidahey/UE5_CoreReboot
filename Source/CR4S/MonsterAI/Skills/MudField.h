#pragma once

#include "CoreMinimal.h"
#include "BaseSkillActor.h"
#include "GameFramework/Actor.h"
#include "MudField.generated.h"

class UNiagaraSystem;
class UTimelineComponent;
class UNiagaraComponent;
class USphereComponent;

UCLASS()
class CR4S_API AMudField : public ABaseSkillActor
{
	GENERATED_BODY()
	
public:	
	AMudField();

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Skill")
	// UNiagaraComponent* NiagaraComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Skill")
	TSubclassOf<AActor> MudActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Skill")
	int32 SpawnCount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Skill", meta=(ClampMin="0.1"))
	float SpawnInterval = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Boss|Skill")
	float SpawnRadius = 300.f;
	
	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float MinSpawnZOffset =  100.f;

	UPROPERTY(EditAnywhere, Category="Boss|Skill")
	float MaxSpawnZOffset = 1000.f;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void SpawnMud();
	
	void SetupNiagara() const;

private:
	FTimerHandle SpawnTimerHandle;
	FString MyHeader = TEXT("MudField");
};
