// WeaponBase.cpp
#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

AWeaponBase::AWeaponBase()
{
	bReplicates = true;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, bIsReloading);
}

void AWeaponBase::StartFire()
{
	
}

void AWeaponBase::StopFire()
{
	
}

void AWeaponBase::Reload()
{
	
}
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}
void AWeaponBase::InitializeWeaponData(FWeaponData* InData)
{
	
}
void AWeaponBase::NotifyAmmoUpdate()
{
	
}