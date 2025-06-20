#pragma once

#include "GameFramework/SaveGame.h"
#include "CoreSaveGame.generated.h"

UCLASS()
class CR4S_API UCoreSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	FVector PlayerLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	FRotator PlayerRotation;
};
