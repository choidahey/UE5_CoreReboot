#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleResourceGimmick.h"
#include "PlanterBoxGimmick.generated.h"

class AGrowableGimmick;
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
	UFUNCTION()
	void OnHarvest();
	
	UPROPERTY(VisibleAnywhere, Category = "Farming")
	TObjectPtr<USceneComponent> SpawnPoint;

	UPROPERTY()
	TObjectPtr<AGrowableGimmick> PlantedGimmick;
	
	bool IsHeldItemSeed() const;
	
#pragma endregion
	
};
