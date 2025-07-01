#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DLWEInteractionHelperComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CR4S_API UDLWEInteractionHelperComponent : public USceneComponent
{
	GENERATED_BODY()

#pragma region USceneComponent

public:
	UDLWEInteractionHelperComponent();

	virtual void BeginPlay() override;
	
#pragma endregion 
	
#pragma region Initialize

public:
	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void CreateDLWEInteractionComponent();

	UFUNCTION(BlueprintCallable, Category = "Initialize")
	void UpdateConnectSlot();

private:
	bool IsReadyToCreate() const;
	void TryCreateDLWEInteractionComponent();
	void ClearDLWEInteractionComponents();

	bool Attach(const int32 Index, USceneComponent* Component) const;
	FName CreateSocketName(const int32 Index) const;
	
	UPROPERTY(VisibleAnywhere, Category = "Initialize")
	TObjectPtr<USkeletalMeshComponent> LegMesh;
	
	UPROPERTY(EditAnywhere, Category = "Initialize")
	TSubclassOf<USceneComponent> DLWEInteractionClass;

	UPROPERTY(EditAnywhere, Category = "Initialize")
	int32 MaxLegCount;

	UPROPERTY(EditAnywhere, Category = "Initialize")
	FName SocketPrefix;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USceneComponent>> DLWEInteractionInstances;

	bool bIsInitialized;
	
#pragma endregion 
};
