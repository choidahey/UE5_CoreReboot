#include "BaseMonster.h"
#include "Controller/BaseMonsterAIController.h"
// #include "Components/MonsterAttributeComponent.h"
// #include "Components/MonsterSkillComponent.h"
// #include "Components/MonsterStateComponent.h"
// #include "Components/MonsterPerceptionComponent.h"
// #include "Components/MonsterAnimComponent.h"

ABaseMonster::ABaseMonster()
{
	PrimaryActorTick.bCanEverTick = true;

	//// Component Initialization
	//AttributeComp = CreateDefaultSubobject<UMonsterAttributeComponent>(TEXT("AttributeComp"));
	//SkillComp = CreateDefaultSubobject<UMonsterSkillComponent>(TEXT("SkillComp"));
	//StateComp = CreateDefaultSubobject<UMonsterStateComponent>(TEXT("StateComp"));
	//PerceptionComp = CreateDefaultSubobject<UMonsterPerceptionComponent>(TEXT("PerceptionComp"));
	//AnimComp = CreateDefaultSubobject<UMonsterAnimComponent>(TEXT("AnimComp"));

	AIControllerClass = ABaseMonsterAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

