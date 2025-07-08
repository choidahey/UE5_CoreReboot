#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractableComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region UActorComponent Override

public:
	UInteractableComponent();
	
	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region Initialize

public:
	void UpdateTraceBlocking(const ECollisionResponse NewResponse = ECR_Block);
	
#pragma endregion
	
#pragma region Interaction

public:
	UFUNCTION(BlueprintCallable, Category = "InteractableComponent|Interaction")
	FORCEINLINE FText GetInteractionText() const { return InteractionText; }

	UFUNCTION(BlueprintCallable, Category = "InteractableComponent|Interaction")
	FORCEINLINE void SetInteractionText(const FText& InInteractionText) { InteractionText = InInteractionText; }
	
	UFUNCTION()
	void ActivateEndOverlapCollision();

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
					UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TryInteract(AActor* Interactor) const;

	void DetectionStateChanged(AActor* DetectingActor, const bool bIsDetected) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionText;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bAllowEndOverlap = false;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (EditCondition = "bAllowEndOverlap", ClampMin = "0.0"))
	float EndOverlapRadius = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (EditCondition = "bAllowEndOverlap", ClampMin = "0.0"))
	bool bDrawDebugSphere = false;

#pragma endregion

#pragma region Highlight

public:
	UFUNCTION(BlueprintCallable, Category = "InteractableComponent|Highlight")
	void SetHighlight(const bool bIsDetected) const;

private:
	UPROPERTY()
	TArray<TObjectPtr<UMeshComponent>> OwnerMeshComponents;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectionStateChanged, AActor*, DetectingActor, const bool, bIsDetected);
	UPROPERTY(BlueprintAssignable, Category = "InteractableComponent|Interaction")
	FOnDetectionStateChanged OnDetectionStateChanged;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryInteract, AActor*, Interactor);
	UPROPERTY(BlueprintAssignable, Category = "InteractableComponent|Interaction")
	FOnTryInteract OnTryInteract;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndSphereOverlap, AActor*, Interactor);
	UPROPERTY(BlueprintAssignable, Category = "InteractableComponent|Interaction")
	FOnEndSphereOverlap OnEndSphereOverlap;
	
#pragma endregion
};
