// Fill out your copyright notice in the Description page of Project Settings.


#include "FirableBase.h"

#include "Outbreak/Util/FloatHelper.h"

AFirableBase::AFirableBase()
{
}

void AFirableBase::StartFire()
{
	if (!CanUse())
		return;
	
	UE_LOG(LogTemp, Log, TEXT("[%s] StartFire"), CURRENT_CONTEXT);
	const float Interval = FloatHelper::RpmToInterval(FirableData.FireRate);
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

void AFirableBase::OnReload(const FOnReloadFinished& DoneCallback)
{
	if (!CanReload())
		return;

	OnReloadFinishedCallback = DoneCallback;
	StartReload();
	UE_LOG(LogTemp, Log, TEXT("[%s] OnReload"), CURRENT_CONTEXT);
}

void AFirableBase::ProcessFire()
{
	CurrentAmmoInMag--;
	UE_LOG(LogTemp, Log, TEXT("[%s] ProcessFire : %d"), CURRENT_CONTEXT, CurrentAmmoInMag);
	if (CurrentAmmoInMag < 0)
	{
		CurrentAmmoInMag = 0;
		StopFire();
	}
}

void AFirableBase::SpawnProjectile()
{
}

void AFirableBase::StartReload()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] StartReload"), CURRENT_CONTEXT);
	// TODO : Manage Reload Duration
	const float ReloadDuration = 2.5f;
	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AFirableBase::FinishReload, ReloadDuration, false);
}

void AFirableBase::FinishReload()
{
	UE_LOG(LogTemp, Log, TEXT("[%s] FinishReload"), CURRENT_CONTEXT);
	const int32 NeededAmmo = FirableData.MagazineSize - CurrentAmmoInMag;
	const int32 AmmoToFill = FMath::Min(NeededAmmo, CurrentTotalAmmo);

	CurrentAmmoInMag += AmmoToFill;
	CurrentTotalAmmo -= AmmoToFill;
	
	bIsReloading = false;
	OnReloadFinishedCallback.ExecuteIfBound();
}