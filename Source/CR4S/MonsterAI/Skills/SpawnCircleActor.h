#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnCircleActor.generated.h"

UCLASS()
class CR4S_API ASpawnCircleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnCircleActor();

	UFUNCTION(BlueprintCallable, Category="Boss|Skill")
	void SpawnInCircle(AActor* InOwnerActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill")
	TSubclassOf<AActor> SpawnActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(ClampMin="0.0"))
	float CircleRadius = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(ClampMin="1"))
	int32 SpawnCount = 8;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(ClampMin="3", ClampMax="256"))
	int32 CircleSegments = 64;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(ClampMin="0.0"))
	float SpawnedLifeTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill")
	bool bActorFaceCenter = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Skill", meta=(ClampMin="0.1"))
	float LineThickness = 2.f;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	AActor* OwnerActor = nullptr;
};
