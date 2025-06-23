#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPoolComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UObjectPoolComponent : public UActorComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
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

	UFUNCTION(BlueprintCallable)
	void ReturnToPoolAfter(float DelaySeconds);

	void SetIsBeingInitialized(bool bInitializing) { bIsBeingInitialized = bInitializing; }

	void HandleReturnToPool();

private:
	bool bIsBeingInitialized = false;
	
	FTimerHandle ReturnTimerHandle;
};