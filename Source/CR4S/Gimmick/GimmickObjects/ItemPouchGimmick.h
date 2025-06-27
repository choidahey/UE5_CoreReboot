#pragma once

#include "CoreMinimal.h"
#include "BaseGimmick.h"
#include "ItemPouchGimmick.generated.h"

class UPlayerInventoryComponent;
class UBaseInventoryItem;
class UBaseInventoryComponent;
class UInteractableComponent;

UCLASS(BlueprintType)
class CR4S_API AItemPouchGimmick : public ABaseGimmick
{
	GENERATED_BODY()

#pragma region ABaseGimmick Override

public:
	AItemPouchGimmick();

	virtual void BeginPlay() override;

	virtual FGimmickSaveGameData GetGimmickSaveGameData_Implementation(bool& bSuccess) override;
	
#pragma endregion

#pragma region UInteractableComponent

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const { return InteractableComponent; }

private:
	UFUNCTION()
	virtual void OnGimmickInteracted(AActor* Interactor);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY()
	TObjectPtr<USoundBase> InteractSound;

#pragma endregion

#pragma region Inventory

public:
	void InitItemPouch(const AActor* SourceActor, const TMap<FName, int32>& RemainingItems) const;

private:
	UFUNCTION()
	void DestroyEmptyItemPouch(int32 NumOccupiedSlots);
	UFUNCTION()
	void UnBoundDelegate();
	void CloseWidget();

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TObjectPtr<UBaseInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> PlayerInventoryComponent;

	bool bIsOpenWidget;

#pragma endregion

#pragma region LaunchItemPouch

public:
	void LaunchItemPouch(const AActor* SourceActor) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "LaunchItemPouch")
	float ForwardImpulseStrength;
	UPROPERTY(EditDefaultsOnly, Category = "LaunchItemPouch")
	float UpImpulseStrength;

#pragma endregion

#pragma region Destroy

public:
	UFUNCTION(BlueprintPure, Category = "ItemPouch|Destroy")
	FORCEINLINE float GetElapsedSeconds() const { return ElapsedSeconds; }

private:
	UFUNCTION()
	void UpdateWorldTime(int64 NewPlayTime);

	UPROPERTY(EditDefaultsOnly, Category = "Destroy")
	float DestroyDelay;

	UPROPERTY(VisibleAnywhere, Category = "Destroy")
	float ElapsedSeconds;

	int64 PrevPlayTime;

#pragma endregion

#pragma region LoadData

public:
	UFUNCTION(BlueprintCallable, Category = "ItemPouch|LoadData")
	void LoadItemPouchData(const float NewElapsedSeconds);

#pragma endregion
};
