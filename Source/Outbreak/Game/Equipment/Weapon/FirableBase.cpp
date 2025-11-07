// Fill out your copyright notice in the Description page of Project Settings.


#include "FirableBase.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
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


void AFirableBase::StartFire()
{
	if (!CanUse())
		return;
	
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
	EquipmentMesh->Stop();
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

 
void AFirableBase::ProcessFire()
{
	CurrentAmmoInMag--;
	if (CurrentAmmoInMag < 0)
	{
		CurrentAmmoInMag = 0;
		StopFire();
	}
	
	OnAmmoChanged.Broadcast(CurrentAmmoInMag, CurrentTotalAmmo);

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
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

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 0.5f);

	if (bHit)
	{
		if (AActor* HitActor = HitResult.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(HitActor, FirableData.Damage, PlayerViewPointRotation.Vector(), HitResult, OwnerController, this, nullptr);
		}
	}

	const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
	if (!OwnerCharacter) return;
	
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;
	
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

void AFirableBase::StartReload(const FOnReloadFinished& DoneCallback)
{
	if (!CanReload())
		return;

	OnReloadFinishedCallback = DoneCallback;
	bIsReloading = true;
	EquipmentMesh->PlayAnimation(CurrentAmmoInMag == 0 ? ReloadEmptyAnim : ReloadAnim, false);
	
	// TODO : Manage Reload Duration
	const float ReloadDuration = 2.0f;
	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AFirableBase::FinishReload, ReloadDuration, false);
}

void AFirableBase::FinishReload()
{
	const int32 NeededAmmo = FirableData.MagazineSize - CurrentAmmoInMag;
	const int32 AmmoToFill = FMath::Min(NeededAmmo, CurrentTotalAmmo);

	CurrentAmmoInMag += AmmoToFill;
	CurrentTotalAmmo -= AmmoToFill;
	
	bIsReloading = false;
	OnReloadFinishedCallback.ExecuteIfBound();
	OnAmmoChanged.Broadcast(CurrentAmmoInMag, CurrentTotalAmmo);
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

EFireType AFirableBase::ToggleFireMode()
{
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