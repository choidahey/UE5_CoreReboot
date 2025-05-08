#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class UInteractionWidget;
class UInteractableComponent;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UInteractionComponent();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Initialize
	
private:
	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	void InitComponent();

	UPROPERTY(visibleanywhere, Category = "Interaction")
	TObjectPtr<APlayerController> OwnerPlayer;
	
#pragma endregion

#pragma region Interaction

public:
	void ShowInteractionWidget() const;

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	bool TryStartInteraction() const;

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent")
	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }

private:
	void FindProcess();

	UPROPERTY(visibleanywhere, Category = "Interaction")
	TObjectPtr<AActor> TargetActor;
	
	UPROPERTY(visibleanywhere, Category = "Interaction")
	TObjectPtr<UInteractableComponent> TargetInteractableComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionRange;

	FHitResult TraceHitResult;

	FTimerHandle FindProcessTimerHandle;

#pragma endregion

#pragma region Interaction Widget

private:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY()
	TObjectPtr<UInteractionWidget> InteractionWidgetInstance;

#pragma endregion
};
