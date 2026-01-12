// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableBase.h"

#include "ThrowableProjectile.h"
#include "GameFramework/Character.h"

AThrowableBase::AThrowableBase()
{
	
}

void AThrowableBase::OnEquip()
{
	
}

void AThrowableBase::OnEndUse()
{
	
}

bool AThrowableBase::CanUse() const
{
	float TimeSinceLastThrow = GetWorld()->GetTimeSeconds() - LastThrowTime;
	return !bIsThrowing && (CurrentAmmo > 0) && (TimeSinceLastThrow >= ThrowCooldown);
}

bool AThrowableBase::IsActive() const
{
	return bIsThrowing;
}

void AThrowableBase::OnUse()
{
	if (!CanUse()) return;
	
	bIsThrowing = true;

	Multicast_ThrowAnim(ThrowableData.ThrowMontage);
}

void AThrowableBase::Multicast_ThrowAnim_Implementation(UAnimMontage* MontageToPlay)
{
	if (ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (MontageToPlay)
		{
			Character->PlayAnimMontage(MontageToPlay);
		}
	}
}

void AThrowableBase::Throw()
{
	if (CurrentAmmo <= 0)
	{
		bIsThrowing = false;
		return;
	}

	if(HasAuthority())
	{
		float TimeNow = GetWorld()->GetTimeSeconds();
		if (TimeNow - LastThrowTime < 0.1f)
		{
			return; 
		}
		if (!ThrowableData.ProjectileClass)
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
		LastThrowTime = TimeNow;
	}
	bIsThrowing = false;	
}





