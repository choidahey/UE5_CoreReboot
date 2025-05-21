#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleResourceGimmick.h"
#include "PlanterBoxGimmick.generated.h"

struct FBaseItemData;
class UInteractableComponent;

UCLASS()
class CR4S_API APlanterBoxGimmick : public ADestructibleResourceGimmick
{
	GENERATED_BODY()

#pragma region ADestructibleResourceGimmick Override

public:
	APlanterBoxGimmick();

	virtual void BeginPlay() override;

#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

protected:
	UFUNCTION()
	virtual void OnGimmickInteracted();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractableComponent> InteractableComponent;

private:
	UFUNCTION()
	void OnDetectionStateChanged(APlayerController* InDetectingPlayerController, bool bInIsDetected);

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	APlayerController* DetectingPlayerController;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	bool bIsDetected;

#pragma endregion

#pragma region Farming

private:
	bool IsHeldItemSeed() const;
	
#pragma endregion
	
#pragma region Delegate

public:
	DECLARE_DELEGATE_OneParam(FOnGrowthStageChanged, int32);
	FOnGrowthStageChanged OnGrowthStageChanged;
	
#pragma endregion
	
};
