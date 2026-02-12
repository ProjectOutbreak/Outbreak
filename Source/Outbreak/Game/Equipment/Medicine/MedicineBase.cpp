// Fill out your copyright notice in the Description page of Project Settings.


#include "MedicineBase.h"

#include "Components/AudioComponent.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"

AMedicineBase::AMedicineBase()
{
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);
	AudioComp->bAutoActivate = false;
}

void AMedicineBase::BeginPlay()
{
	Super::BeginPlay();
}


bool AMedicineBase::CanUse() const
{
	if (bIsUsing) return false;

	if (const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner()))
	{
		if (const ADefaultPlayerState* PS = OwnerCharacter->GetPlayerState<ADefaultPlayerState>())
		{
			return PS->GetMedicineCount(MedicineData.MedicineType) > 0;
		}
	}
	return false;
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
		
		if (ADefaultPlayerState* PS = Player->GetPlayerState<ADefaultPlayerState>())
		{
			PS->ConsumeMedicine(MedicineData.MedicineType, 1);
            
			if (PS->GetMedicineCount(MedicineData.MedicineType) <= 0)
			{
				if (UEquipmentController* EqController = Player->FindComponentByClass<UEquipmentController>())
				{
					EqController->RemoveEquipment(this);
				}
			}
		}
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
	if (AudioComp && MedicineData.HealSound)
	{
		AudioComp->SetSound(MedicineData.HealSound);
		AudioComp->Play();
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
	if (AudioComp && AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}
}

UTexture2D* AMedicineBase::GetEquipmentIcon() const
{
	{
		if (MedicineData.ItemIcon.IsNull())
		{
			return nullptr;
		}
    
		UTexture2D* LoadedIcon = MedicineData.ItemIcon.LoadSynchronous();

    
		return LoadedIcon;
	}
}


