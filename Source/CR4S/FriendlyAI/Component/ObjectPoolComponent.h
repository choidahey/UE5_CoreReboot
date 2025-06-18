#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UObjectPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObjectPoolComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolSettings")
	int32 InitialPoolSize = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PoolSettings")
	bool bAutoRegisterOnBeginPlay = true;

	UFUNCTION(BlueprintCallable)
	virtual void OnSpawnFromPool();
	
	UFUNCTION(BlueprintCallable)
	virtual void OnReturnToPool();

	void SetIsBeingInitialized(bool bInitializing) { bIsBeingInitialized = bInitializing; }

protected:
	virtual void BeginPlay() override;

private:
	bool bIsBeingInitialized = false;
};