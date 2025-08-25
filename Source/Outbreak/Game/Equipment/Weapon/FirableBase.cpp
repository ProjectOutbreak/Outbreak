// Fill out your copyright notice in the Description page of Project Settings.


#include "FirableBase.h"

#include "Outbreak/Util/FloatHelper.h"

AFirableBase::AFirableBase()
{
}

void AFirableBase::StartFire()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] StartFire"), CURRENT_CONTEXT);
	float Interval = FloatHelper::RpmToInterval(FirableData.FireRate);
	switch (CurrentFireType)
	{
	case EFireType::Single:
		ProcessFire();
		break;
	case EFireType::Burst:
		// TODO : implement burst fire
		// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFirableBase::ProcessFire, Interval, true, 0.0f);
		break;
	case EFireType::Auto:
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFirableBase::ProcessFire, Interval, true, 0.0f);
		break;
	case EFireType::None:
		break;
	}
}

void AFirableBase::StopFire()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] StopFire"), CURRENT_CONTEXT);
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AFirableBase::Reload()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] Reload"), CURRENT_CONTEXT);
}

void AFirableBase::ProcessFire()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] ProcessFire"), CURRENT_CONTEXT);
}

void AFirableBase::SpawnProjectile()
{
}

void AFirableBase::StartReload()
{
}

void AFirableBase::FinishReload()
{
}