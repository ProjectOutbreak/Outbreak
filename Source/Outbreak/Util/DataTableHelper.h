#pragma once
#include "Define.h"

class DataTableHelper
{
public:
	template<typename T>
	static bool GetDataFromDataTable(UDataTable* DataTable, const FName& InRowId, T& OutData, const FString& FunctionName = TEXT("GetDataFromDataTable"))
	{
		if (!DataTable)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] DataTable is null"), CURRENT_CONTEXT);
			return false;
		}

		const T* FoundRow = DataTable->FindRow<T>(InRowId, *FunctionName);
		if (!FoundRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Row '%s' not found in data table"), CURRENT_CONTEXT, *InRowId.ToString());
			return false;
		}
		
		OutData = *FoundRow;
		return true;
	}
};
