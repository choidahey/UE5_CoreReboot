// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThunderWall.generated.h"

class UNiagaraSystem;
class UBoxComponent;
class UNiagaraComponent;

UCLASS()
class CR4S_API AThunderWall : public AActor
{
	GENERATED_BODY()

public:
	AThunderWall();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallSkill")
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WallSkill")
	UBoxComponent* CollisionComp;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallSkill")
	// UNiagaraSystem* WallNiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallSkill|Dimensions")
	FVector WallExtent = FVector(50.f, 500.f, 800.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallSkill|Damage")
	float Damage = 0.f;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);
};
