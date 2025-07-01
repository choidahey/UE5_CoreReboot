#include "HelperBotSoundNotify.h"
#include "../BaseHelperBot.h"
#include "../Data/HelperBotSoundData.h"
#include "Components/SkeletalMeshComponent.h"

UHelperBotSoundNotify::UHelperBotSoundNotify()
{
    SoundPropertyName = TEXT("HitSound");
}

void UHelperBotSoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp)
        return;

    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance)
        return;

    APawn* Pawn = AnimInstance->TryGetPawnOwner();
    ABaseHelperBot* HelperBot = Cast<ABaseHelperBot>(Pawn);
    if (!HelperBot || !HelperBot->SoundData)
        return;
    
    const UStruct* SoundDataStruct = HelperBot->SoundData->GetClass();
    
    for (TFieldIterator<FProperty> PropertyIt(SoundDataStruct); PropertyIt; ++PropertyIt)
    {
        FProperty* Property = *PropertyIt;
        if (Property->GetName() == SoundPropertyName)
        {
            if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
            {
                if (ObjectProperty->PropertyClass->IsChildOf(USoundBase::StaticClass()))
                {
                    void* PropertyValuePtr = Property->ContainerPtrToValuePtr<void>(HelperBot->SoundData);
                    if (PropertyValuePtr)
                    {
                        USoundBase* Sound = Cast<USoundBase>(ObjectProperty->GetObjectPropertyValue(PropertyValuePtr));
                        if (Sound)
                        {
                            HelperBot->PlayBotSound(Sound, HelperBot->GetActorLocation());
                        }
                    }
                    return;
                }
            }
        }
    }
}

TArray<FString> UHelperBotSoundNotify::GetSoundOptions() const
{
    TArray<FString> Options;
    
    for (TFieldIterator<FProperty> PropertyIt(UHelperBotSoundData::StaticClass()); PropertyIt; ++PropertyIt)
    {
        FProperty* Property = *PropertyIt;
        if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
        {
            if (ObjectProperty->PropertyClass->IsChildOf(USoundBase::StaticClass()))
            {
                Options.Add(Property->GetName());
            }
        }
    }
    
    return Options;
}