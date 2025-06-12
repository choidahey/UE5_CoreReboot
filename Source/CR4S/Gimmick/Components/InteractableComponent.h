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
	void UpdateTraceBlocking(const ECollisionResponse NewResponse = ECR_Block) const;
	
#pragma endregion
	
#pragma region Interaction

public:
	void TryInteract(AActor* Interactor) const;

	void DetectionStateChanged(AActor* DetectingActor, const bool bIsDetected) const;
	
	UFUNCTION(BlueprintCallable, Category = "InteractableComponent|Interaction")
	FORCEINLINE FText GetInteractionText() const { return InteractionText; }

	UFUNCTION(BlueprintCallable, Category = "InteractableComponent|Interaction")
	FORCEINLINE void SetInteractionText(const FText& InInteractionText) { InteractionText = InInteractionText; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> InteractionTraceChannel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractionText;
	
#pragma endregion

#pragma region Highlight

public:
	void InitHighlightMaterial();

	void SetHighlight(const bool bIsDetected) const;
	void ChangeHighlightColor(const FColor& InHighlightColor) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> HighlightMaterialInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	FName HighlightOpacityParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	FName HighlightColorParamName;

	UPROPERTY(EditDefaultsOnly, Category = "Highlight")
	FColor DefaultHighlightColor;
	
#pragma endregion

#pragma region Delegate

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDetectionStateChanged, AActor*, DetectingActor, const bool, bIsDetected);
	UPROPERTY(BlueprintAssignable, Category = "InteractableComponent|Interaction")
	FOnDetectionStateChanged OnDetectionStateChanged;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryInteract, AActor*, Interactor);
	UPROPERTY(BlueprintAssignable, Category = "InteractableComponent|Interaction")
	FOnTryInteract OnTryInteract;
	
#pragma endregion
};
