// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableBase.h"

#include "ThrowableProjectile.h"
#include "GameFramework/Character.h"

AThrowableBase::AThrowableBase()
{
	
}
void AThrowableBase::BeginPlay()
{
	
}

void AThrowableBase::OnEquip()
{
	
}

bool AThrowableBase::CanUse() const
{
	return !bIsThrowing && CurrentAmmo > 0;
}

bool AThrowableBase::IsActive() const
{
	return bIsThrowing;
}

void AThrowableBase::OnUse()
{
	if (!CanUse()) return;
	
	bIsThrowing = true;

	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (ThrowableData.ThrowMontage)
		{
			Character->PlayAnimMontage(ThrowableData.ThrowMontage);
		}
	}
}

void AThrowableBase::OnEndUse()
{
	
}

void AThrowableBase::Throw()
{
	if (!ThrowableData.ProjectileClass || CurrentAmmo <= 0)
	{
		bIsThrowing = false;
		return;
	}

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	CurrentAmmo--;

	FVector CameraLocation;
	FRotator CameraRotation;
	OwnerCharacter->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector SpawnLocation = CameraLocation + (CameraRotation.Vector() * 50.0f);	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	AThrowableProjectile* Projectile = GetWorld()->SpawnActor<AThrowableProjectile>(
		ThrowableData.ProjectileClass,
		SpawnLocation,
		CameraRotation,
		SpawnParams
		);

	if (Projectile)
	{
		Projectile->InitializeProjectile(CameraRotation.Vector(), ThrowableData);
	}
	bIsThrowing = false;

	
}





