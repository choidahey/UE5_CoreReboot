#pragma once

#include "GameFramework/SaveGame.h"
#include "CoreSaveGame.generated.h"

UCLASS()
class CR4S_API UCoreSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, Category = "Player")
	FVector PlayerLocation;
	UPROPERTY(VisibleAnywhere, Category = "Player")
	FRotator PlayerRotation;
};
