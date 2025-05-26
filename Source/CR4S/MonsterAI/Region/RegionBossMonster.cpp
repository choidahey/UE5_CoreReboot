#include "RegionBossMonster.h"
#include "MonsterAI/Controller/RegionBossMonsterAIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationInvokerComponent.h"

ARegionBossMonster::ARegionBossMonster()
	: MyHeader(TEXT("RegionBossMonster"))
{
	PrimaryActorTick.bCanEverTick = false;

	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetCollisionObjectType(ECC_Pawn);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Block);
	SkeletalMesh->SetGenerateOverlapEvents(false);

	NavGenerationRadius = 2000.0f;
	NavRemovalRadius = 2500.0f;

	NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvoker"));
	NavInvoker->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);


}
