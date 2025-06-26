#include "BaseHelperBot.h"
#include "BaseHelperBot.h"

#include "BrainComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/InGame/HelperBotStateManagerWidget.h"
#include "Controller/HelperBotAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HelperBotStatsSubsystem.h"
#include "../Gimmick/Components/InteractableComponent.h"
#include "Components/SplineComponent.h" 
#include "UI/HelperBotInfoWidget.h"
#include "Inventory/Components/PlayerInventoryComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "UI/InGame/SurvivalHUD.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/AIJumpComponent.h"
#include "Components/PointLightComponent.h"
#include "Inventory/Components/BaseInventoryComponent.h"
#include "Inventory/UI/InventoryContainerWidget.h"
#include "Particles/ParticleSystemComponent.h"
#include "Character/Characters/PlayerCharacter.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/WidgetComponent.h"


ABaseHelperBot::ABaseHelperBot()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractableComp = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComp"));

	InventoryComponent = CreateDefaultSubobject<UBaseInventoryComponent>(TEXT("InventoryComponent"));

	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvokerComponent"));
	

	LeftEyeWorkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("LeftEyeWorkSpline"));
	LeftEyeWorkSpline->SetupAttachment(RootComponent);

	RightEyeWorkSpline = CreateDefaultSubobject<USplineComponent>(TEXT("RightEyeWorkSpline"));
	RightEyeWorkSpline->SetupAttachment(RootComponent);

	LeftEyeWorkVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftEyeWorkVFX"));
	LeftEyeWorkVFXComponent->SetupAttachment(LeftEyeWorkSpline);
	LeftEyeWorkVFXComponent->SetAutoActivate(false);
	LeftEyeWorkVFXComponent->SetVisibility(false);

	RightEyeWorkVFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightEyeWorkVFX"));
	RightEyeWorkVFXComponent->SetupAttachment(RightEyeWorkSpline);
	RightEyeWorkVFXComponent->SetAutoActivate(false);
	RightEyeWorkVFXComponent->SetVisibility(false);

	LeftEyeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("LeftEyeLight"));
	LeftEyeLight->SetupAttachment(GetMesh(), TEXT("LeftEye"));

	RightEyeLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("RightEyeLight"));
	RightEyeLight->SetupAttachment(GetMesh(), TEXT("RightEye"));

	WorkTargetParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WorkTargetParticle"));
	WorkTargetParticle->SetAutoActivate(false);

	HitEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HitEffectComponent"));
	HitEffectComponent->SetupAttachment(RootComponent);
	HitEffectComponent->SetAutoActivate(false);
	HitEffectComponent->SetVisibility(false);

	InfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InfoWidgetComponent"));
	InfoWidgetComponent->SetupAttachment(RootComponent);
	InfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InfoWidgetComponent->SetDrawAtDesiredSize(true);
	InfoWidgetComponent->SetVisibility(false);
}

void ABaseHelperBot::BeginPlay()
{
	Super::BeginPlay();

	LoadStats();
	UpdateStateVisualEffects();
	
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetAvoidanceEnabled(true);
	}

	if (InteractableComp)
	{
		InteractableComp->OnTryInteract.AddUniqueDynamic(this, &ABaseHelperBot::HandleInteract);
		InteractableComp->OnDetectionStateChanged.AddUniqueDynamic(this, &ABaseHelperBot::OnDetectedChange);
		InteractableComp->SetInteractionText(FText::FromString(BotName));
	}
	
	if (IsValid(InfoWidgetComponent))
	{
		InfoUIInstance = Cast<UHelperBotInfoWidget>(InfoWidgetComponent->GetUserWidgetObject());
		if (IsValid(InfoUIInstance))
		{
			InfoUIInstance->SetOwnerHelperBot(this);
		}
		InfoWidgetComponent->SetVisibility(false);
	}
}

void ABaseHelperBot::LoadStats()
{
	if (UHelperBotStatsSubsystem* Subsys = GetGameInstance()->GetSubsystem<UHelperBotStatsSubsystem>())
	{
		Subsys->GetStatsRowAsync(RowName, [this](const FHelperBotStatsRow* Row)
		{
			if (!Row)
			{
				UE_LOG(LogTemp, Warning, TEXT("[HelperBot] Stats Load Failed"));
				return;
			}

			CurrentStats = *Row;

			if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
			{
				MoveComp->MaxWalkSpeed = CurrentStats.WalkSpeed;
				MoveComp->JumpZVelocity = CurrentStats.JumpHeight;
			}
			if (UAIJumpComponent* JumpComp = FindComponentByClass<UAIJumpComponent>())
			{
				JumpComp->UpdateOwnerStats();
			}

			if (!PickUpData.bIsInit)
			{
				CurrentHealth = CurrentStats.MaxHealth;
				PickUpData.bIsInit = true;
			}
			else
			{
				CurrentHealth = PickUpData.CurrentHealth;
				BotName = PickUpData.BotName;
			}

			if (IsValid(InteractableComp))
			{
				InteractableComp->SetInteractionText(FText::FromString(BotName));
			}

		});
	}
}

void ABaseHelperBot::OnDetectedChange(AActor* InteractableActor, bool bIsDetected)
{
	if (bIsDetected)
	{
		if (IsValid(InfoWidgetComponent))
		{
			InfoUIInstance = Cast<UHelperBotInfoWidget>(InfoWidgetComponent->GetUserWidgetObject());
			if (IsValid(InfoUIInstance))
			{
				InfoUIInstance->SetOwnerHelperBot(this);
				InfoWidgetComponent->SetVisibility(true);
			}
		}
	}
	else
	{
		if (IsValid(InfoWidgetComponent))
		{
			InfoWidgetComponent->SetVisibility(false);
		}
	}
}

void ABaseHelperBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseHelperBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseHelperBot::HandleInteract(AActor* InteractableActor)
{
	AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController());
	if (BotAI)
	{
		EHelperBotState CurrentState = static_cast<EHelperBotState>(
			BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
		
		if (CurrentState == EHelperBotState::Dead)
		{
			return;
		}
	}

	if (StateUIInstance && StateUIInstance->IsInViewport())
	{
		StateUIInstance->RemoveFromParent();
		StateUIInstance = nullptr;

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
			{
				HUD->SetInputMode(ESurvivalInputMode::GameOnly, nullptr, false);
			}
		}
		return;
	}

	if (bIsWorking)
	{
		SetIsWorking(false);
		StopEyeBeamWork();
	}

	if (BotAI)
	{
		if (StateUIClass)
		{
			if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				if (ASurvivalHUD* HUD = Cast<ASurvivalHUD>(PC->GetHUD()))
				{
					StateUIInstance = HUD->CreateAndAddWidget<UHelperBotStateManagerWidget>(
						StateUIClass, 15, ESlateVisibility::Visible);

					if (StateUIInstance)
					{
						EHelperBotState CurrentState = static_cast<EHelperBotState>(BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
						StateUIInstance->InitializeWithController(BotAI, CurrentState);
						HUD->SetInputMode(ESurvivalInputMode::GameAndUI, StateUIInstance, true);
					}
				}
			}
		}
	}
}

void ABaseHelperBot::UpdateEyeBeamWorkTarget(AActor* TargetActor)
{
	if (!LeftEyeWorkSpline || !RightEyeWorkSpline || !TargetActor) 
	{
		return;
	}
	
	if (!LeftEyeWorkVFXComponent || !RightEyeWorkVFXComponent)
	{
		return;
	}
	
	if (!LeftEyeWorkVFXComponent->GetAsset())
	{
		return;
	}
	
	FVector LeftEyeStart = LeftEyeLight ? LeftEyeLight->GetComponentLocation() : GetActorLocation();
	FVector RightEyeStart = RightEyeLight ? RightEyeLight->GetComponentLocation() : GetActorLocation();

	FVector TargetLocation = TargetActor->GetActorLocation();
	const FVector End = FVector(TargetLocation.X, TargetLocation.Y, LeftEyeStart.Z);

	LeftEyeWorkSpline->ClearSplinePoints(false);
	LeftEyeWorkSpline->AddSplinePoint(LeftEyeStart, ESplineCoordinateSpace::World, false);
	LeftEyeWorkSpline->AddSplinePoint(End, ESplineCoordinateSpace::World, true);
	
	LeftEyeWorkVFXComponent->SetVariableObject(FName("SplineComponent"), LeftEyeWorkSpline);
	LeftEyeWorkVFXComponent->SetVariableVec3(FName("StartLocation"), LeftEyeStart);
	LeftEyeWorkVFXComponent->SetVariableVec3(FName("EndLocation"), End);
	
	float LeftSplineLength = LeftEyeWorkSpline->GetSplineLength();
	LeftEyeWorkVFXComponent->SetVariableFloat(FName("SplineLength"), LeftSplineLength);
	LeftEyeWorkVFXComponent->SetVisibility(true);
	LeftEyeWorkVFXComponent->Activate(true);

	RightEyeWorkSpline->ClearSplinePoints(false);
	RightEyeWorkSpline->AddSplinePoint(RightEyeStart, ESplineCoordinateSpace::World, false);
	RightEyeWorkSpline->AddSplinePoint(End, ESplineCoordinateSpace::World, true);
	
	if (RightEyeWorkVFXComponent->GetAsset() != LeftEyeWorkVFXComponent->GetAsset())
	{
		RightEyeWorkVFXComponent->SetAsset(LeftEyeWorkVFXComponent->GetAsset());
	}
	
	RightEyeWorkVFXComponent->SetVariableObject(FName("SplineComponent"), RightEyeWorkSpline);
	RightEyeWorkVFXComponent->SetVariableVec3(FName("StartLocation"), RightEyeStart);
	RightEyeWorkVFXComponent->SetVariableVec3(FName("EndLocation"), End);
	
	float RightSplineLength = RightEyeWorkSpline->GetSplineLength();
	RightEyeWorkVFXComponent->SetVariableFloat(FName("SplineLength"), RightSplineLength);
	RightEyeWorkVFXComponent->SetVisibility(true);
	RightEyeWorkVFXComponent->Activate(true);
}

void ABaseHelperBot::StopEyeBeamWork()
{
	if (LeftEyeWorkVFXComponent)
	{
		LeftEyeWorkVFXComponent->Deactivate();
	}
	
	if (RightEyeWorkVFXComponent)
	{
		RightEyeWorkVFXComponent->Deactivate();
	}
}

void ABaseHelperBot::UpdateStateVisualEffects()
{
	FLinearColor TargetEyeColor = IdleEyeColor;
	UNiagaraSystem* TargetVFXAsset = nullptr;
	
	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		EHelperBotState CurrentState = static_cast<EHelperBotState>(
			BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
		
		switch (CurrentState)
		{
		case EHelperBotState::ChopWood:
		case EHelperBotState::Mining:
		case EHelperBotState::Gathering:
			TargetEyeColor = ResourceEyeColor;
			TargetVFXAsset = ResourceWorkVFX;
			break;
			
		case EHelperBotState::Defending:
			TargetEyeColor = DefendingEyeColor;
			TargetVFXAsset = DefendingWorkVFX;
			break;
			
		case EHelperBotState::Repairing:
			TargetEyeColor = RepairingEyeColor;
			TargetVFXAsset = RepairingWorkVFX;
			break;
			
		default:
			TargetEyeColor = IdleEyeColor;
			TargetVFXAsset = nullptr;
			break;
		}
	}
	
	if (LeftEyeLight)
	{
		LeftEyeLight->SetLightColor(TargetEyeColor);
	}
	if (RightEyeLight)
	{
		RightEyeLight->SetLightColor(TargetEyeColor);
	}
	
	if (LeftEyeWorkVFXComponent)
	{
		LeftEyeWorkVFXComponent->SetAsset(TargetVFXAsset);
	}
	if (RightEyeWorkVFXComponent)
	{
		RightEyeWorkVFXComponent->SetAsset(TargetVFXAsset);
	}
}

float ABaseHelperBot::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		EHelperBotState CurrentState = static_cast<EHelperBotState>(
			BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
		
		if (CurrentState == EHelperBotState::Dead)
		{
			return 0.0f;
		}
	}

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHealth -= ActualDamage;

	if (DamageCauser)
	{
		FVector HitDirection = (GetActorLocation() - DamageCauser->GetActorLocation()).GetSafeNormal();
		PlayHitEffect(HitDirection);
	}

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		StartDeathSequence();
	}

	return ActualDamage;
}

void ABaseHelperBot::PlayHitEffect(const FVector& HitDirection)
{
	if (HitEffectComponent && HitEffectComponent->GetAsset())
	{
		FVector HitLocation = GetActorLocation() + HitDirection * 50.0f;
		HitEffectComponent->SetWorldLocation(HitLocation);
		HitEffectComponent->SetVisibility(true);
		HitEffectComponent->Activate(true);
	}
	
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetBodySimulatePhysics(FName("Spine"), true);
		
		FVector ImpulseForce = HitDirection * 150000.0f;
		MeshComp->AddImpulseAtLocation(ImpulseForce, GetActorLocation(), FName("Spine"));
		
		FTimerHandle PhysicsTimer;
		GetWorldTimerManager().SetTimer(PhysicsTimer, [this, MeshComp]()
		{
			MeshComp->SetBodySimulatePhysics(FName("Spine"), false);
		}, 0.2f, false);
	}
}

void ABaseHelperBot::StartDeathSequence()
{
	if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
	{
		BotAI->SetBotState(EHelperBotState::Dead);
		BotAI->GetBrainComponent()->StopLogic(TEXT("Dead"));
	}

	SetActorEnableCollision(false);
	StartFadeOut();

	if (DeathMontage)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(DeathMontage);
		}
	}
}

void ABaseHelperBot::SetBotName(const FString& NewName)
{
	if (NewName.Len() >= 2 && NewName.Len() <= 8)
	{
		BotName = NewName;
		
		if (InteractableComp)
		{
			InteractableComp->SetInteractionText(FText::FromString(BotName));
		}
	}
}

void ABaseHelperBot::SetPickUpData(const FHelperPickUpData& InPickUpData)
{ 
	PickUpData = InPickUpData;
}

#pragma region FadeEffect
void ABaseHelperBot::StartFadeOut()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;
    
	for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
	{
		UMaterialInstanceDynamic* DynMat = MeshComp->CreateAndSetMaterialInstanceDynamic(i);
		if (DynMat)
		{
			if (AHelperBotAIController* BotAI = Cast<AHelperBotAIController>(GetController()))
			{
				EHelperBotState CurrentState = static_cast<EHelperBotState>(
					BotAI->GetBlackboardComponent()->GetValueAsEnum(FName("HelperBotState")));
				
				if (CurrentState == EHelperBotState::Dead)
				{
					DynMat->SetScalarParameterValue(TEXT("Appearance"), 1.0f);
					DynMat->SetVectorParameterValue(TEXT("Param"), DeadColor);
				}
				else
				{
					DynMat->SetScalarParameterValue(TEXT("Appearance"), 1.0f);
					DynMat->SetVectorParameterValue(TEXT("Param"), PickUpColor);
				}
			}
		}
	}
	
	MeshComp->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
	{
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
		
	FTimerDelegate FadeDelegate = FTimerDelegate::CreateUObject(this, &ABaseHelperBot::UpdateFadeOut);
	GetWorldTimerManager().SetTimer(FadeTimerHandle, FadeDelegate, 0.02f, true);
	SetLifeSpan(2.f);
}

void ABaseHelperBot::UpdateFadeOut()
{
	ElapsedFadeTime += 0.02f;
	float NewAppearance = FMath::Lerp(1.0f, 0.0f, ElapsedFadeTime / 2.0f);

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		for (int32 i = 0; i < MeshComp->GetNumMaterials(); ++i)
		{
			if (UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(i)))
			{
				DynMat->SetScalarParameterValue(TEXT("Appearance"), NewAppearance);
			}
		}
	}

	if (ElapsedFadeTime >= 2.0f)
	{
		GetWorldTimerManager().ClearTimer(FadeTimerHandle);
	}
}
#pragma endregion

#pragma region RobotRepair
bool ABaseHelperBot::CanRepair(APlayerCharacter* Player) const
{
	if (CurrentHealth >= CurrentStats.MaxHealth)
	{
		return false;
	}
	
	if (!Player)
	{
		return false;
	}
	
	UPlayerInventoryComponent* PlayerInventory = Player->FindComponentByClass<UPlayerInventoryComponent>();
	if (!PlayerInventory)
	{
		return false;
	}

	for (const auto& RequiredItem : CurrentStats.RepairRequiredItems)
	{
		FName ItemName = RequiredItem.Key;
		int32 RequiredCount = RequiredItem.Value;
		
		if (PlayerInventory->GetItemCountByRowName(ItemName) < RequiredCount)
		{
			return false;
		}
	}
	
	return true;
}

bool ABaseHelperBot::RepairBot(APlayerCharacter* Player)
{
	if (!CanRepair(Player))
	{
		return false;
	}
	
	UPlayerInventoryComponent* PlayerInventory = Player->FindComponentByClass<UPlayerInventoryComponent>();
	if (!PlayerInventory)
	{
		return false;
	}
	
	for (const auto& RequiredItem : CurrentStats.RepairRequiredItems)
	{
		FName ItemName = RequiredItem.Key;
		int32 RequiredCount = RequiredItem.Value;
		
		PlayerInventory->RemoveItemByRowName(ItemName, RequiredCount);
	}
	
	CurrentHealth = CurrentStats.MaxHealth;
	
	return true;
}
#pragma endregion