#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseGimmick.generated.h"

UCLASS()
class CR4S_API ABaseGimmick : public AActor
{
	GENERATED_BODY()

#pragma region AActor Override
public:
	ABaseGimmick();
	
#pragma endregion

#pragma region Components

public:
	UFUNCTION(BlueprintPure, Category = "Gimmick|Components")
	FORCEINLINE UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneComponent;

	
#pragma endregion

#pragma region Resources

// public:
//
// 	UFUNCTION(BlueprintCallable, Category = "Gimmick|Variables")
// 	void GetResources(TArray<FResourceItemData>& OutResources);
// 	
// private:
// 	UPROPERTY(VisibleAnywhere, Category = "Variables")
// 	TArray<FResourceItemData> Resources;
	
#pragma endregion
};
