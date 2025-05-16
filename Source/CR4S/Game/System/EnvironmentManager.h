#pragma once

#include "GameFramework/Actor.h"
#include "EnvironmentManager.generated.h"

UCLASS()
class CR4S_API AEnvironmentManager : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintImplementableEvent, Category = "Sky")
	void SetSkyTime(float Time);

protected:
	AEnvironmentManager();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:

};
