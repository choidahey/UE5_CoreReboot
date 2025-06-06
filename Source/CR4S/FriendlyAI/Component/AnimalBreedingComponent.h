#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../BaseAnimal.h"
#include "AnimalBreedingComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UAnimalBreedingComponent : public UActorComponent
{
	GENERATED_BODY()

// public:
// 	UAnimalBreedingComponent();
// 	
// 	UFUNCTION(BlueprintCallable)
// 	void StartGestation(AActor* Partner = nullptr);
//
// protected:
// 	virtual void BeginPlay() override;
//
// private:
// 	ABaseAnimal* OwnerAnimal;
// 	bool bIsPregnant = false;
// 	AActor* PartnerActor = nullptr;
// 	AActor* OffspringActor = nullptr;
//
// 	float GestationPeriod = 0.f;
// 	float GrowthDuration = 0.f;
//
// 	static constexpr float OffspringInitialScale = 0.2f;
// 	static constexpr float OffspringAdultScale   = 1.0f;
//
// 	float ElapsedGrowthTime = 0.f;
//
// 	FTimerHandle GestationTimerHandle;
// 	FTimerHandle GrowthTimerHandle;
//
// 	void DoBirth();
// 	void StartGrowth();
// 	void UpdateGrowth();
// 	void Log(const FString& Message) const;
};