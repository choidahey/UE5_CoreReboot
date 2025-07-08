#pragma once

#include "GameFramework/Actor.h"
#include "Game/System/SeasonType.h"
#include "DemoAltar.generated.h"

class UInteractableComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UAltarWidget;
class AEnvironmentManager;
class ASeasonBossMonster;

UCLASS()
class CR4S_API ADemoAltar : public AActor
{
	GENERATED_BODY()
	
public:	
	ADemoAltar();
	virtual void BeginPlay() override;

#pragma region Components

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInteractableComponent* InteractableComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* WidgetComponent;

#pragma endregion

#pragma region Widget Class

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UAltarWidget> AltarWidgetClass;

#pragma endregion

#pragma region Widget Handlers
protected:
	UFUNCTION()
	void HandleTryInteract(AActor* Interactor);
	UFUNCTION()
	void HandleEndInteract(AActor* Interactor);

private:

	void ShowAlterUI();
	void HIdeAlterUI();

	bool bIsActive;

#pragma endregion

#pragma region Internal Logic
public:
	void RequestChangeWeather(ESeasonType Season);
	void SummonFrostBoss();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<ASeasonBossMonster> FrostBossClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float TransitionTime = 5.0f;
	
	TObjectPtr<AEnvironmentManager> EnvironmentManager;

#pragma endregion
};
