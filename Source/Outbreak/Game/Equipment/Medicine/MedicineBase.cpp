// Fill out your copyright notice in the Description page of Project Settings.


#include "MedicineBase.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

bool AMedicineBase::CanUse() const
{
	return !bIsUsing;
}

void AMedicineBase::OnUse()
{
	if (bIsUsing || !CanUse()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot use medicine now (Already using or Cooldown)"));
		return;
	}
	UseSelf();
}
void AMedicineBase::OnEndUse()
{
	if (!HasAuthority()) return;

	bIsUsing = false;
	GetWorldTimerManager().ClearTimer(UseTimer);

	if (MedicineData.HealMontage)
	{
		Multicast_StopHealAnim(MedicineData.HealMontage);
	}
}

void AMedicineBase::OnEquip()
{
}

void AMedicineBase::UseSelf()
{
	if (!HasAuthority()) return;

	ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return;

	bIsUsing = true;
    
	if (MedicineData.HealMontage)
	{
		Multicast_PlayHealAnim(MedicineData.HealMontage);
	}

	float Duration = (MedicineData.HealMontage) ? MedicineData.HealMontage->GetPlayLength() : 0.0f;
	float WaitTime = (Duration > 0.0f) ? Duration : (MedicineData.UseTime > 0.0f ? MedicineData.UseTime : 1.0f);
    
	GetWorldTimerManager().SetTimer(UseTimer, this, &AMedicineBase::OnUseComplete, WaitTime, false);
}

void AMedicineBase::UseOnTarget(TObjectPtr<class ACharacterPlayer> TargetCharacter)
{
}

void AMedicineBase::OnUseComplete()
{
	bIsUsing = false;
	GetWorldTimerManager().ClearTimer(UseTimer);

	if (ACharacterPlayer* Player = Cast<ACharacterPlayer>(GetOwner()))
	{
		ApplyHealEffect(Player);
	}
	OnEndUse();
}

void AMedicineBase::ApplyHealEffect(TObjectPtr<class ACharacterPlayer> TargetCharacter)
{
	if (TargetCharacter)
	{
		TargetCharacter->ApplyHeal(MedicineData.HealAmount);
	}
}

void AMedicineBase::Multicast_PlayHealAnim_Implementation(UAnimMontage* MontageToPlay)
{
	ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (OwnerCharacter && MontageToPlay)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(MontageToPlay);
		}
	}
}

void AMedicineBase::Multicast_StopHealAnim_Implementation(UAnimMontage* MontageToStop)
{
	ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (OwnerCharacter && MontageToStop)
	{
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Stop(0.2f, MontageToStop);
		}
	}
}


