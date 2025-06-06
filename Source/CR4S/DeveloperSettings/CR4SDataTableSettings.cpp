#include "CR4SDataTableSettings.h"

UDataTable* UCR4SDataTableSettings::GetDataTableByName(const FName DataTableName) const
{
	return GetDataByName<UDataTable>(DataTableName, DataTables);
}

UDataAsset* UCR4SDataTableSettings::GetDataAssetByName(const FName DataAssetName) const
{
	return GetDataByName<UDataAsset>(DataAssetName, DataAssets);
}
