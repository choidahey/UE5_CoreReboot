#pragma once

#include "CoreMinimal.h"
#include "CR4S.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"

#include "CR4SDataTableSettings.generated.h"

UCLASS(config=CR4SDataTables, defaultconfig, meta=(DisplayName="CR4S Data Tables"))
class CR4S_API UCR4SDataTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	UDataTable* GetDataTableByName(const FName DataTableName) const;

	UFUNCTION()
	UDataAsset* GetDataAssetByName(const FName DataAssetName) const;
	
protected:
	template<typename T>
	T* GetDataByName(const FName DataName, const TMap<FName, TSoftObjectPtr<T>>& DataRef) const
	{
		T* Data = nullptr;
		if (!DataRef[DataName].ToSoftObjectPath().IsValid())
		{
			CR4S_Log(LogSetting, Warning, TEXT("%s is not set"), *DataName.ToString());
			return nullptr;
		}
	
		if (!DataRef[DataName].IsValid())
		{
			Data = DataRef[DataName].LoadSynchronous();

			if (!IsValid(Data))
			{
				CR4S_Log(LogSetting, Warning, TEXT("%s load failed"), *DataName.ToString());
				return nullptr;
			}
			
			CR4S_Log(LogSetting, Warning, TEXT("%s load success"), *DataName.ToString());
		}
		else
		{
			Data = DataRef[DataName].Get();
		}

		return Data;
	}
	
	UPROPERTY(EditAnywhere, Config, Category = "DataTable", meta = (AllowedClasses = "/Script/Engine.DataTable"))
	TMap<FName, TSoftObjectPtr<UDataTable>> DataTables;

	UPROPERTY(EditAnywhere, Config, Category = "DataAsset", meta = (AllowedClasses = "/Script/Engine.DataAsset"))
	TMap<FName, TSoftObjectPtr<UDataAsset>> DataAssets;
};
