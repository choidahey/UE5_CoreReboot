#include "LIHCheatPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "BaseAnimal.h"
#include "BaseHelperBot.h"
#include "Controller/HelperBotAIController.h"
#include "Component/AnimalBreedingComponent.h"
#include "Kismet/GameplayStatics.h"


ALIHCheatPlayerController::ALIHCheatPlayerController()
{
    bShowMouseCursor = true;
}

void ALIHCheatPlayerController::CMD_SpawnAnimal(const FString& AnimalBPPath, int32 Count)
{
    UClass* AnimalClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *AnimalBPPath));
    if (!AnimalClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("CMD_SpawnAnimal : Check BP Path '%s'"), *AnimalBPPath);
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FVector Origin = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
    FActorSpawnParameters Params;
    Params.Owner = GetPawn();

    for (int32 i = 0; i < Count; ++i)
    {
        FVector Offset = FVector(FMath::RandRange(-200,200), FMath::RandRange(-200,200), 0);
        FVector SpawnLoc = Origin + Offset;
        ABaseAnimal* NewAnimal = World->SpawnActor<ABaseAnimal>(AnimalClass, SpawnLoc, FRotator::ZeroRotator, Params);
        if (NewAnimal)
        {
            UE_LOG(LogTemp, Log, TEXT("CMD_SpawnAnimal : Spawned %s at %s"), *NewAnimal->GetName(), *SpawnLoc.ToString());
        }
    }
}

void ALIHCheatPlayerController::CMD_DamageAll(float DamageAmount)
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ABaseAnimal> It(World); It; ++It)
    {
        ABaseAnimal* Animal = *It;
        if (Animal)
        {
            UGameplayStatics::ApplyDamage(Animal, DamageAmount, this, this, nullptr);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("CMD_DamageAll : Applied %.1f damage to all animals"), DamageAmount);
}

void ALIHCheatPlayerController::CMD_StunAll(float StunAmount)
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ABaseAnimal> It(World); It; ++It)
    {
        ABaseAnimal* Animal = *It;
        if (Animal)
        {
            Animal->ApplyStun(StunAmount);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("CMD_StunAll : Applied %.1f stun to all animals"), StunAmount);
}

void ALIHCheatPlayerController::CMD_KillAll()
{
    UWorld* World = GetWorld();
    if (!World) return;

    int32 Count = 0;

    for (TActorIterator<ABaseAnimal> It(World); It; ++It)
    {
        ABaseAnimal* Animal = *It;
        if (Animal && Animal->CurrentState != EAnimalState::Dead)
        {
            Animal->Die();
            ++Count;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("CMD_KillAll: Killed %d animals"), Count);
}

void ALIHCheatPlayerController::CMD_ImpregnateAll()
{
    UWorld* World = GetWorld();
    if (!World) return;

    for (TActorIterator<ABaseAnimal> It(World); It; ++It)
    {
        ABaseAnimal* Animal = *It;
        if (!Animal || !Animal->bIsFemale) 
            continue;

        if (UAnimalBreedingComponent* BreedComp = Animal->FindComponentByClass<UAnimalBreedingComponent>())
        {
            BreedComp->StartGestation(nullptr);
            UE_LOG(LogTemp, Log, TEXT("CMD_ImpregnateAll : %s is now pregnant"), *Animal->GetName());
        }
    }
}

void ALIHCheatPlayerController::CMD_ChangeHelperBotState(EHelperBotState NewState)
{
    for (TActorIterator<ABaseHelperBot> It(GetWorld()); It; ++It)
    {
        ABaseHelperBot* Bot = *It;
        if (Bot && Bot->GetController())
        {
            if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(Bot->GetController()))
            {
                BotAI->SetBotState(NewState);
            }
        }
    }
}

void ALIHCheatPlayerController::CMD_ToggleHelperBotUI()
{
    if (CheatWidgetInstance && CheatWidgetInstance->IsInViewport())
    {
        CheatWidgetInstance->RemoveFromParent();
        CheatWidgetInstance = nullptr;
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
        return;
    }
    
    for (TActorIterator<ABaseHelperBot> It(GetWorld()); It; ++It)
    {
        if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(It->GetController()))
        {
            CheatWidgetInstance = CreateWidget<UHelperBotStateManagerWidget>(this, CheatWidgetClass);
            if (CheatWidgetInstance)
            {
                CheatWidgetInstance->InitializeWithController(BotAI);
                CheatWidgetInstance->AddToViewport();
                SetInputMode(FInputModeGameAndUI());
                bShowMouseCursor = true;
            }
            break;
        }
    }
}