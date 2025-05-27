#include "MonsterAI/Animations/AnimNotify/AnimNotify_BlizzardSkill.h"
#include "MonsterAI/Skills/Blizzard.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "MonsterAI/BaseMonster.h"

UAnimNotify_BlizzardSkill::UAnimNotify_BlizzardSkill()
	: MyHeader(TEXT("Notify_BlizzardSkill"))
{
}

void UAnimNotify_BlizzardSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !BlizzardSkillClass)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Missing MeshComp or BlizzardClass"), *MyHeader);
		return;
	}

	ABaseMonster* MonsterCharacter = Cast<ABaseMonster>(MeshComp->GetOwner());
	if (!MonsterCharacter) return;

	const FVector SpawnLocation = MonsterCharacter->GetActorLocation();
	const FRotator SpawnRotation = MonsterCharacter->GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = MonsterCharacter;
	SpawnParams.Instigator = MonsterCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABlizzard* Blizzard = MonsterCharacter->GetWorld()->SpawnActor<ABlizzard>(
		BlizzardSkillClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Blizzard)
	{
		Blizzard->Launch(MonsterCharacter);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Invaild Blizzard Actor"), *MyHeader);
	}
}
