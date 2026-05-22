// Fill out your copyright notice in the Description page of Project Settings.


#include "FirableBase.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"
#include "Outbreak/Util/EnumHelper.h"
#include "Outbreak/Util/FloatHelper.h"

AFirableBase::AFirableBase()
{
}


void AFirableBase::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsRecoiling)
	{
		RecoverRecoil(DeltaSeconds);
	}
}
void AFirableBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFirableBase, CurrentFireType);
	DOREPLIFETIME(AFirableBase, CurrentAmmoInMag);
}

void AFirableBase::OnRep_CurrentAmmoInMag()
{
	OnPlayerAmmoChangedDelegate.Broadcast();
}

void AFirableBase::StartFire()
{
	if (!CanUse())
		return;

	bIsFiring = true;
	
	const float Interval = FloatHelper::RpmToInterval(FirableData.FireRate);
	switch (CurrentFireType)
	{
	case EFireType::Single:
		EquipmentMesh->PlayAnimation(FireAnim, false);
		ProcessFire();
		break;
	case EFireType::Burst:
		// TODO : implement burst fire
		// GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFirableBase::ProcessFire, Interval, true, 0.0f);
		break;
	case EFireType::Auto:
		EquipmentMesh->PlayAnimation(FireAnim, true);
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AFirableBase::ProcessFire, Interval, true, 0.0f);
		break;
	case EFireType::None:
		break;
	}
}

void AFirableBase::StopFire()
{
	bIsFiring = false;
	EquipmentMesh->Stop();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AFirableBase::ProcessFire()
{
	if (HasAuthority())
	{
		CurrentAmmoInMag--;
		if (CurrentAmmoInMag <= 0)
		{
			CurrentAmmoInMag = 0;
			StopFire();
		}
		// 서버에서도 UI 갱신이 필요하면 호출
		OnPlayerAmmoChangedDelegate.Broadcast();
	}
	AController* OwnerController = GetInstigatorController();
	if (!OwnerController) return;

	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	OwnerController->GetPlayerViewPoint(PlayerViewPointLocation, PlayerViewPointRotation);

	const FVector Start = PlayerViewPointLocation;
	const FVector End = Start + PlayerViewPointRotation.Vector() * 10000.0f;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bReturnPhysicalMaterial = true;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, QueryParams);
	if (HasAuthority() || OwnerController->IsLocalPlayerController())
	{
		Multicast_PlayFireEffects(GetActorLocation(), HitResult);
		if (bHit)
		{
			if (HasAuthority())
			{
				ApplyDamageToTarget(OwnerController, HitResult);
			}
			else if (OwnerController->IsLocalPlayerController())
			{
				Server_ProcessHit(HitResult);
			}
		}
	}
	if (HasAuthority())
	{
		Client_ApplyRecoil();
	}
	else 
	{
		ApplyRecoilLogic(); 
	}
}

void AFirableBase::StartReload(const FOnReloadFinished& DoneCallback)
{
	if (!CanReload())
		return;

	OnReloadFinishedCallback = DoneCallback;
	bIsReloading = true;
	EquipmentMesh->PlayAnimation(CurrentAmmoInMag == 0 ? ReloadEmptyAnim : ReloadAnim, false);

	if (HasAuthority())
	{
		bool bIsReloadEmpty = (CurrentAmmoInMag == 0);
		Multicast_PlayReloadAnim(bIsReloadEmpty);
	}
	// TODO : Manage Reload Duration
	const float ReloadDuration = 2.0f;
	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AFirableBase::FinishReload, ReloadDuration, false);
}

bool AFirableBase::CanReload() const
{
	return !bIsReloading && CurrentAmmoInMag < FirableData.MagazineSize && GetReservedAmmo() > 0;
}

void AFirableBase::FinishReload()
{
	ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return;
    
	ADefaultPlayerState* PS = OwnerCharacter->GetPlayerState<ADefaultPlayerState>();
	if (!PS) return;

	const int32 ReserveAmmo = PS->GetReserveAmmo(FirableData.FirableType);

	const int32 NeededAmmo = FirableData.MagazineSize - CurrentAmmoInMag;
	const int32 AmmoToFill = FMath::Min(NeededAmmo, ReserveAmmo);
	
	CurrentAmmoInMag += AmmoToFill;

	if (HasAuthority())
	{
		PS->ConsumeAmmo(FirableData.FirableType, AmmoToFill);
	}
	
	bIsReloading = false;
	OnReloadFinishedCallback.ExecuteIfBound();
	OnPlayerAmmoChangedDelegate.Broadcast();
}

void AFirableBase::ApplyDamageToTarget(AController* InstigatorController, const FHitResult& HitResult)
{
	if (!HasAuthority()) return;

	if (AActor* HitActor = HitResult.GetActor())
	{
		if (IZombieInterface* HitZombie = Cast<IZombieInterface>(HitActor))
		{
			UGameplayStatics::ApplyPointDamage(HitActor, FirableData.Damage, HitResult.Location, HitResult, InstigatorController, this, nullptr);
		}
	}
}

void AFirableBase::Server_ProcessHit_Implementation(const FHitResult& HitResult)
{
	ApplyDamageToTarget(GetInstigatorController(), HitResult);
}

void AFirableBase::Multicast_PlayFireEffects_Implementation(const FVector MuzzleLocation, const FHitResult& HitResult)
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
	}
}
void AFirableBase::Client_ApplyRecoil_Implementation()
{
	ApplyRecoilLogic();
}
void AFirableBase::ApplyRecoilLogic()
{
	const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return;
    
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC || !PC->IsLocalController()) return;

	const float HorizontalRecoil = UKismetMathLibrary::RandomFloatInRange(-FirableData.HorizontalRecoil, FirableData.HorizontalRecoil);

	PC->AddPitchInput(-FirableData.VerticalRecoil);
	PC->AddYawInput(HorizontalRecoil);

	bIsRecoiling = true;

	GetWorldTimerManager().ClearTimer(RecoilResetTimer);
	GetWorldTimerManager().SetTimer(RecoilResetTimer, FTimerDelegate::CreateLambda([&]()
	{
	   bIsRecoiling = false;
	}), RecoilRecoveryTime, false);
}
void AFirableBase::Multicast_PlayReloadAnim_Implementation(bool bIsReloadEmpty)
{
	if (EquipmentMesh)
	{
		UAnimationAsset* AnimToPlay = bIsReloadEmpty ? ReloadEmptyAnim : ReloadAnim;
		if (AnimToPlay)
		{
			EquipmentMesh->PlayAnimation(AnimToPlay, false);
		}
	}
}
void AFirableBase::RecoverRecoil(const float DeltaTime)
{
	const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return;
	
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;
	
	const float RecoveryAmount = (FirableData.VerticalRecoil / RecoilRecoveryTime) * DeltaTime;
	PC->AddPitchInput(RecoveryAmount);
}

int32 AFirableBase::GetReservedAmmo() const
{
	const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return 0;

	const ADefaultPlayerState* PS = OwnerCharacter->GetPlayerState<ADefaultPlayerState>();
	if (!PS) return 0;

	const int32 ReserveAmmo = PS->GetReserveAmmo(FirableData.FirableType);

	return ReserveAmmo;
}

EFireType AFirableBase::ToggleFireMode()
{
	if (!HasAuthority())
	{
		Server_ToggleFireMode();
		return CurrentFireType;
	}
	// TODO : Single, Burst, Auto 세 가지 타입 가능하게 수정
	for (int32 i = 0; i < FirableData.FireTypes.Num(); i++)
	{
		if (FirableData.FireTypes[i] == CurrentFireType)
			continue;

		CurrentFireType = FirableData.FireTypes[i];
		UE_LOG(LogTemp, Log, TEXT("[%s] Toggled Fire Mode to: %s"), CURRENT_CONTEXT, *EnumHelper::EnumToString(CurrentFireType));
		break;
	}
	
	return CurrentFireType;
}

bool AFirableBase::IsActive() const
{
	return bIsReloading || bIsFiring;
}

void AFirableBase::Server_ToggleFireMode_Implementation()
{
	ToggleFireMode();
}
