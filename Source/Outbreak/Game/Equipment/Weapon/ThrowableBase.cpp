// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableBase.h"

#include "ThrowableProjectile.h"
#include "GameFramework/Character.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"

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
	ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (OwnerCharacter)
	{
		if (ADefaultPlayerState* PS = OwnerCharacter->GetPlayerState<ADefaultPlayerState>())
		{
			float TimeSinceLastThrow = GetWorld()->GetTimeSeconds() - LastThrowTime;
			return !bIsThrowing && (PS->GetThrowableCount(ThrowableData.ThrowableType) > 0 && (TimeSinceLastThrow >= ThrowCooldown));
		}
	}
	return false;
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
    ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
    if (!OwnerCharacter) return;

    ADefaultPlayerState* PS = OwnerCharacter->GetPlayerState<ADefaultPlayerState>();
    if (!PS) return;
	
    int32 CurrentCount = PS->GetThrowableCount(ThrowableData.ThrowableType);
    if (CurrentCount <= 0)
    {
        bIsThrowing = false;
        return;
    }
    if (HasAuthority())
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
    	
        if (PS->ConsumeThrowable(ThrowableData.ThrowableType, 1))
        {
            FVector CameraLocation;
            FRotator CameraRotation;
            
            if (OwnerCharacter->GetController())
            {
                OwnerCharacter->GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);
            }
            else
            {
                CameraLocation = OwnerCharacter->GetActorLocation();
                CameraRotation = OwnerCharacter->GetActorRotation();
            }

            FVector SpawnLocation = CameraLocation + (CameraRotation.Vector() * 50.0f);
            
            FActorSpawnParameters SpawnParams;
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
    }
	bIsThrowing = false;
}