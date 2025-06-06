#pragma once

#include "CoreMinimal.h"
#include "Gimmick/GimmickObjects/DestructibleGimmick.h"
#include "BarnGimmick.generated.h"

class UInteractableComponent;
class UBarnManagementWidget;

UCLASS()
class CR4S_API ABarnGimmick : public ADestructibleGimmick
{
	GENERATED_BODY()

public:
	ABarnGimmick();

	virtual void BeginPlay() override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BarnMeshComponent;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UBarnManagementWidget> BarnUIClass;

	UFUNCTION()
	void OnInteract(AActor* Interactor);

public:
	void SetBarnUIInstance(UBarnManagementWidget* InInstance) { BarnUIInstance = InInstance; }

protected:
	UPROPERTY()
	TObjectPtr<UBarnManagementWidget> BarnUIInstance = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Barn|Animals")
	TMap<FName, TSubclassOf<AActor>> AnimalBlueprintMap;

public:
	void SpawnAnimalByRowName(FName RowName);
};
