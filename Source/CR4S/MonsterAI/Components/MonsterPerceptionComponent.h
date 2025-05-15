#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "MonsterPerceptionComponent.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorPerceptionSignature, AActor*, Actor);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CR4S_API UMonsterPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
public:
	UMonsterPerceptionComponent();

	UPROPERTY(BlueprintAssignable, Category = "Perception")
	FOnActorPerceptionSignature OnPlayerDetected;

	UPROPERTY(BlueprintAssignable, Category = "Perception")
	FOnActorPerceptionSignature OnPlayerLost;

	UFUNCTION(BlueprintCallable, Category = "Perception")
	AActor* GetDetectedPlayerBySight() const;
	UFUNCTION(BlueprintCallable, Category = "Perception")
	AActor* GetDetectedPlayerByHearing() const;

protected:
	UFUNCTION()
	void OnUpdatePerception(AActor* Actor, FAIStimulus Stimulus);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Perception")
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(EditAnywhere, Category = "Perception")
	UAISenseConfig_Hearing* HearingConfig;

private:
	AActor* FindTaggedActorInSense(const TSubclassOf<UAISense> SenseClass, const FName& Tag) const;

	FString MyHeader;
};
