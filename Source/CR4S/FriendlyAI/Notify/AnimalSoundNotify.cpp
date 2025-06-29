#include "AnimalSoundNotify.h"
#include "../BaseAnimal.h"
#include "Components/SkeletalMeshComponent.h"

UAnimalSoundNotify::UAnimalSoundNotify()
{
    SoundArrayName = TEXT("HitSounds");
}

void UAnimalSoundNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!MeshComp)
        return;

    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance)
        return;

    APawn* Pawn = AnimInstance->TryGetPawnOwner();
    ABaseAnimal* Animal = Cast<ABaseAnimal>(Pawn);
    if (!Animal || !Animal->SoundData)
        return;
    
    const UStruct* SoundDataStruct = Animal->SoundData->GetClass();
    
    for (TFieldIterator<FProperty> PropertyIt(SoundDataStruct); PropertyIt; ++PropertyIt)
    {
        FProperty* Property = *PropertyIt;
        if (Property->GetName() == SoundArrayName)
        {
            if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
            {
                if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(ArrayProperty->Inner))
                {
                    if (ObjectProperty->PropertyClass->IsChildOf(USoundBase::StaticClass()))
                    {
                        void* ContainerPtr = Property->ContainerPtrToValuePtr<void>(Animal->SoundData);
                        if (!ContainerPtr)
                            return;

                        FScriptArrayHelper ArrayHelper(ArrayProperty, ContainerPtr);
                        
                        TArray<USoundBase*> SoundArray;
                        
                        for (int32 i = 0; i < ArrayHelper.Num(); ++i)
                        {
                            if (USoundBase* Sound = Cast<USoundBase>(ObjectProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(i))))
                            {
                                SoundArray.Add(Sound);
                            }
                        }
                        
                        if (SoundArray.Num() > 0)
                        {
                            Animal->PlayAnimalSound(SoundArray, Animal->GetActorLocation(), ConcurrencyType, Pitch, StartTime);
                        }
                        return;
                    }
                }
            }
        }
    }
}