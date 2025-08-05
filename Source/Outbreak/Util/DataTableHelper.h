#pragma once
#include "Define.h"

class DataTableHelper
{
public:
	template<typename T>
	static bool GetDataFromDataTable(const TObjectPtr<UDataTable> InDataTable, const FName& InRowId, T& OutData, const FString& FunctionName = TEXT("GetDataFromDataTable"))
	{
		if (!InDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] DataTable is null"), CURRENT_CONTEXT);
			return false;
		}

		const T* FoundRow = InDataTable->FindRow<T>(InRowId, *FunctionName);
		if (!FoundRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Row '%s' not found in data table"), CURRENT_CONTEXT, *InRowId.ToString());
			return false;
		}
		
		OutData = *FoundRow;
		return true;
	}

	template <typename T>
	static void LoadDataTableToMap(const TObjectPtr<UDataTable> InDataTable, TMap<FString, T*>& OutMap)
	{
		if (!InDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] DataTable is null"), CURRENT_CONTEXT);
			return;
		}

		const TMap<FName, uint8*> RowMap = InDataTable->GetRowMap();

		for (const TPair<FName, uint8*>& Row : RowMap)
		{
			if (T* TypedRow = reinterpret_cast<T*>(Row.Value))
			{
				OutMap.Add(Row.Key.ToString(), TypedRow);
			}
		}
	}
};
