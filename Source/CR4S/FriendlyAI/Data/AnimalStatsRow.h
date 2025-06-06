#pragma once

#include "Engine/DataTable.h"
#include "AnimalStatsRow.generated.h"

USTRUCT(BlueprintType)
struct FAnimalStatsRow : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BehaviorType = TEXT("");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxHealth = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float JumpPower = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackDamage = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackRange = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AttackInterval = 0.0f;
        
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StunAmount = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ChargeSpeed = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ChargeDistance = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WalkSpeed = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RunSpeed = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxStamina = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StaminaRegenRate = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StunThreshold = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StunDuration = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SightRadius = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HearingRadius = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PeripheralVisionAngle = 0.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TargetLostRange = 0.0f;

    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // uint8 bCanBeTamed = 0;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // float Mood = 0.0f;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // float Hunger = 0.0f;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // uint8 bIsMale = 0;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // float GestationPeriod = 0.0f;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // int32 OffspringCount = 0;
    //
    // UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // float GrowthTime = 0.0f;
};
