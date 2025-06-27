// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTool.h"

#include "CR4S.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Utility/DataLoaderSubsystem.h"


// Sets default values
APlayerTool::APlayerTool()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMeshComp=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent=StaticMeshComp;

}

void APlayerTool::Initialize(APlayerCharacter* OwnerCharacter)
{
	OwningCharacter=OwnerCharacter;
}

void APlayerTool::SetGameplayTag(const FGameplayTag GameplayTag)
{
	Super::SetGameplayTag(GameplayTag);
	
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)) return;

	UGameInstance* GI=OwningCharacter->GetGameInstance();
	if (!GI) return;

	UDataLoaderSubsystem* DataLoader=GI->GetSubsystem<UDataLoaderSubsystem>();
	if (!DataLoader||!ToolTag.IsValid()) return;

	const bool bSuccess=DataLoader->LoadToolInfoByTag(ToolTag,PlayerToolInfo);
	
	if (CR4S_ENSURE(LogHong1,bSuccess))
	{
		StaticMeshComp->SetStaticMesh(PlayerToolInfo.StaticMesh);
	}
	else
	{
		PlayerToolInfo=FPlayerToolInfo();
		StaticMeshComp->SetStaticMesh(nullptr);
	}
}

UMeshComponent* APlayerTool::GetToolMeshComponent()
{
	return StaticMeshComp;
}

void APlayerTool::OnAttack()
{
	if (!CR4S_ENSURE(LogHong1,OwningCharacter)||!CR4S_ENSURE(LogHong1,PlayerToolInfo.AttackMontage)) return;
	
	OwningCharacter->PlayAnimMontage(PlayerToolInfo.AttackMontage,MontagePlayRate);
}

float APlayerTool::ComputeFinalDamage()
{
	return PlayerToolInfo.AttackPower;
}

// Called when the game starts or when spawned
void APlayerTool::BeginPlay()
{
	Super::BeginPlay();
	OwningCharacter=Cast<APlayerCharacter>(GetOwner());
	
}

// Called every frame
void APlayerTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

