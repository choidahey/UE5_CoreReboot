// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBullet.h"
#include "HomingBullet.generated.h"

UCLASS()
class CR4S_API AHomingBullet : public ABaseBullet
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHomingBullet();

#pragma region Override
public:
	virtual void Initialize(const FBulletInfo& InData, const float InDamage, AActor* HomingTarget=nullptr) override;
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
#pragma endregion

protected:
	void ActivateHoming();
	
#pragma region Cached
private:
	UPROPERTY(VisibleAnywhere, Category="Cached")
	TWeakObjectPtr<AActor> HomingTargetActor;
	
	UPROPERTY(VisibleAnywhere, Category="Cached")
	FTimerHandle HomingActivationTimerHandle;
#pragma endregion
};
