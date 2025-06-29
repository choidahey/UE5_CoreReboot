#include "FriendlyAI/Data/HelperBotSoundData.h"

TArray<FString> UHelperBotSoundData::GetSoundPropertyNames() const
{
	TArray<FString> PropertyNames;
    
	for (TFieldIterator<FProperty> PropertyIt(UHelperBotSoundData::StaticClass()); PropertyIt; ++PropertyIt)
	{
		FProperty* Property = *PropertyIt;
		if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			if (ObjectProperty->PropertyClass->IsChildOf(USoundBase::StaticClass()))
			{
				PropertyNames.Add(Property->GetName());
			}
		}
	}
    
	return PropertyNames;
}