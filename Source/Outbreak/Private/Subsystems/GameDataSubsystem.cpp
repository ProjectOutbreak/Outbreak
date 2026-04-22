// Fill out your copyright notice in the Description page of Project Settings.

#include "Subsystems/GameDataSubsystem.h"
#include "Data/OutbreakDeveloperSettings.h"
#include "Util/DataTableHelper.h"
#include "Util/Define.h"
#include "Util/EnumHelper.h"

const FZombieData* UGameDataSubsystem::GetZombieData(const EZombieSubType InSubType) const
{
	const FString RowName = EnumHelper::EnumToString(InSubType);
	if (ZombieDataMap.Contains(RowName))
	{
		return ZombieDataMap[RowName];
	}
	
	UE_LOG(LogTemp, Error, TEXT("[%s] No Zombie data found for type: %s"), CURRENT_CONTEXT, *RowName);
	return nullptr;
}

void UGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const UOutbreakDeveloperSettings* Settings = UOutbreakDeveloperSettings::Get())
	{
		if (!Settings->ZombieDataTable.IsNull())
		{
			const TObjectPtr<UDataTable> ZombieDataTable = Settings->ZombieDataTable.LoadSynchronous();
			DataTableHelper::LoadDataTableToMap(ZombieDataTable, ZombieDataMap);
		}
	}
}
