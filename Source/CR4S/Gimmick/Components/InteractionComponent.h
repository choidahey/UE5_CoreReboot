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
	void InitComponent();
	
	UPROPERTY(Visibleanywhere, Category = "Initialize")
	TObjectPtr<APlayerController> OwnerPlayerController;
	
#pragma endregion

#pragma region Interaction

public:
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent|Intercation")
	bool TryStartInteraction() const;

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent|Intercation")
	FORCEINLINE AActor* GetDetectedActor() const { return DetectedActor; }

private:
	void UpdateDetectProcess();
	bool TryDetectProcess();

	UPROPERTY(Visibleanywhere, Category = "Interaction")
	TObjectPtr<AActor> DetectedActor;
	
	UPROPERTY(Visibleanywhere, Category = "Interaction")
	TObjectPtr<UInteractableComponent> DetectedInteractableComponent;
	
	UPROPERTY(EditAnywhere, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel;
	
	/**
	 * Trace Distance
	 * From Camera to TargetActor
	 */
	UPROPERTY(EditAnywhere, Category = "Interaction")
	float HitResultTraceDistance;
	
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

#pragma region Debug

public:
	UFUNCTION(BlueprintCallable, Category = "InteractionComponent|Debug")
	FORCEINLINE bool GetIsDebugMode() const { return bIsDebugMode; }

	UFUNCTION(BlueprintCallable, Category = "InteractionComponent|Debug")
	FORCEINLINE void ChangeDebugMode(const bool bInIsDebugMode) { bIsDebugMode = bInIsDebugMode; }

private:
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bIsDebugMode;
	
#pragma endregion
};
