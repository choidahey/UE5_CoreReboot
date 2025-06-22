// BTTask_PlayMontage.cpp
#include "BTTask_PlayAnyMontage.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/Engine.h"

UBTTask_PlayAnyMontage::UBTTask_PlayAnyMontage()
{
    NodeName = TEXT("PlayAnyMontage");
    bNotifyTick = true;
    bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_PlayAnyMontage::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    FBTTask_PlayMontageMemory* MyMemory = reinterpret_cast<FBTTask_PlayMontageMemory*>(NodeMemory);
    MyMemory->Reset();

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: No AI Controller"));
        return EBTNodeResult::Failed;
    }

    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: No Character"));
        return EBTNodeResult::Failed;
    }

    if (!MontageToPlay)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: No Montage to Play"));
        return EBTNodeResult::Failed;
    }

    USkeletalMeshComponent* MeshComp = Character->GetMesh();
    if (!MeshComp)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: No Skeletal Mesh Component"));
        return EBTNodeResult::Failed;
    }

    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: No Anim Instance"));
        return EBTNodeResult::Failed;
    }

    // 이미 같은 몽타주가 재생 중이면 중지
    if (AnimInstance->Montage_IsPlaying(MontageToPlay))
    {
        AnimInstance->Montage_Stop(0.2f, MontageToPlay);
    }

    // 이동/회전 제한 설정
    UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
    if (MovementComp)
    {
        MyMemory->bPreviousMaxSpeed = MovementComp->GetMaxSpeed() > 0.0f;
        MyMemory->bPreviousAllowRotation = MovementComp->bUseControllerDesiredRotation;
        
        if (!bAllowMovement)
        {
            MovementComp->MaxWalkSpeed = 0.0f;
        }
        
        if (!bAllowRotation)
        {
            MovementComp->bUseControllerDesiredRotation = false;
        }
    }

    // 몽타주 재생
    float MontageDuration = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, 0.0f);
    
    if (MontageDuration <= 0.0f)
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: Failed to play montage"));
        return EBTNodeResult::Failed;
    }

    // 시작 섹션 설정
    if (StartSectionName != NAME_None)
    {
        AnimInstance->Montage_JumpToSection(StartSectionName, MontageToPlay);
    }

    // 메모리 설정
    MyMemory->PlayingMontage = MontageToPlay;
    MyMemory->MontageStartTime = GetWorld()->GetTimeSeconds();
    MyMemory->MontageDuration = MontageDuration;
    MyMemory->bMontageFinished = false;

    // 몽타주 완료 델리게이트 바인딩
    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &UBTTask_PlayAnyMontage::OnMontageEnded);
    AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);

    UE_LOG(LogBehaviorTree, Log, TEXT("BTTask_PlayMontage: Started playing montage %s (Duration: %f)"), 
           *MontageToPlay->GetName(), MontageDuration);

    // 완료까지 기다리지 않으면 즉시 성공
    if (!bWaitForFinish)
    {
        return EBTNodeResult::Succeeded;
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_PlayAnyMontage::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (!bWaitForFinish)
    {
        return;
    }

    FBTTask_PlayMontageMemory* MyMemory = reinterpret_cast<FBTTask_PlayMontageMemory*>(NodeMemory);
    
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
    if (!Character)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 몽타주가 완료되었는지 확인
    if (MyMemory->bMontageFinished || !AnimInstance->Montage_IsPlaying(MyMemory->PlayingMontage))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    // 타임아웃 체크 (안전장치)
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - MyMemory->MontageStartTime;
    
    if (ElapsedTime > MyMemory->MontageDuration + 1.0f) // 1초 여유
    {
        UE_LOG(LogBehaviorTree, Warning, TEXT("BTTask_PlayMontage: Montage timed out"));
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}

void UBTTask_PlayAnyMontage::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    FBTTask_PlayMontageMemory* MyMemory = reinterpret_cast<FBTTask_PlayMontageMemory*>(NodeMemory);
    
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (AIController)
    {
        ACharacter* Character = Cast<ACharacter>(AIController->GetPawn());
        if (Character)
        {
            UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
            
            // 몽타주 중지 (필요한 경우)
            if (bStopMontageOnAbort && AnimInstance && MyMemory->PlayingMontage)
            {
                if (AnimInstance->Montage_IsPlaying(MyMemory->PlayingMontage))
                {
                    AnimInstance->Montage_Stop(0.2f, MyMemory->PlayingMontage);
                }
            }

            // 이동/회전 제한 복원
            UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
            if (MovementComp)
            {
                if (!bAllowMovement && MyMemory->bPreviousMaxSpeed)
                {
                    MovementComp->MaxWalkSpeed = 600.f; // 기본값으로 복원 (프로젝트에 맞게 조정)
                }
                
                if (!bAllowRotation)
                {
                    MovementComp->bUseControllerDesiredRotation = MyMemory->bPreviousAllowRotation;
                }
            }
        }
    }

    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

uint16 UBTTask_PlayAnyMontage::GetInstanceMemorySize() const
{
    return sizeof(FBTTask_PlayMontageMemory);
}

void UBTTask_PlayAnyMontage::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    UE_LOG(LogBehaviorTree, Log, TEXT("BTTask_PlayMontage: Montage ended %s (Interrupted: %s)"), 
           Montage ? *Montage->GetName() : TEXT("None"), 
           bInterrupted ? TEXT("True") : TEXT("False"));
}