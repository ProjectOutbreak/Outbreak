#include "WeaponAR.h"
#include "UObject/ConstructorHelpers.h"
#include "ARAmmo.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/Character/Zombie/CharacterZombie.h"
#include "Outbreak/UI/OBHUD.h"

AWeaponAR::AWeaponAR()
{
    AmmoClass = AARAmmo::StaticClass();
    
    MuzzleSocketName = TEXT("Muzzle_AR");
    
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> WeaponMeshObj(
        TEXT("/Game/FPS_Weapon_Pack/SkeletalMeshes/AR2/SM_weapon_AR2.SM_weapon_AR2"));
    if (WeaponMeshObj.Succeeded())
    {
        WeaponMesh->SetSkeletalMesh(WeaponMeshObj.Object);
    }
    WeaponMesh->SetHiddenInGame(true);
    
    static ConstructorHelpers::FObjectFinder<USoundBase> tempSound(TEXT("/Script/Engine.SoundWave'/Game/Audio/SFX/AR_Single.AR_Single'"));
    if (tempSound.Succeeded()) {
        WeaponData.ShotSound = tempSound.Object; 
    }
    static ConstructorHelpers::FObjectFinder<UDataTable> DT_WeaponData(TEXT("/Script/Engine.DataTable'/Game/Data/WeaponDataTable.WeaponDataTable'"));
    if (DT_WeaponData.Succeeded())
    {
        WeaponDataTable = DT_WeaponData.Object;
    }
    static ConstructorHelpers::FObjectFinder<UNiagaraSystem> FireEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Art/VFX/MuzzleFlash/MuzzleFlash/Niagara/NS_MuzzleFlash.NS_MuzzleFlash'"));
    if (FireEffect.Succeeded())
    {
        NiagaraMuzzleFlash = FireEffect.Object;
    }
}

void AWeaponAR::BeginPlay()
{
    Super::BeginPlay();
    
    if (!HasAuthority())
        return;

    static const FString ContextString(TEXT("WeaponAR DataTable Initialization"));
    
    FWeaponData* FoundRow = WeaponDataTable->FindRow<FWeaponData>(FName(TEXT("WeaponAR")), ContextString);
    if (FoundRow)
    {
        InitializeWeaponData(FoundRow);
    }
}

void AWeaponAR::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AWeaponAR, WeaponData);

}

void AWeaponAR::Reload()
{
    if (bIsReloading || WeaponData.CurrentAmmo == WeaponData.MagazineCapacity || WeaponData.TotalAmmo <= 0)
        return;

    if (!HasAuthority())
    {
        ServerReload();
    }

    bIsReloading = true;
    StopFire();

    GetWorldTimerManager().SetTimer(
        ReloadTimerHandle,
        this,
        &AWeaponAR::FinishReload,
        WeaponData.ReloadDuration,
        false
    );
    
    NotifyAmmoUpdate();
}

void AWeaponAR::FinishReload()
{
    const int32 Needed = WeaponData.MagazineCapacity - WeaponData.CurrentAmmo;
    const int32 ToReload = FMath::Min(Needed, WeaponData.TotalAmmo);

    WeaponData.CurrentAmmo += ToReload;
    WeaponData.TotalAmmo -= ToReload;
    bIsReloading = false;
    
    NotifyAmmoUpdate();
}

void AWeaponAR::StartFire()
{
    if (bIsReloading)
    {
        StopFire();
        return;
    }

    if (WeaponData.CurrentAmmo <= 0)
    {
        Reload();
        return;
    }
    
    ServerStartFire();
}

void AWeaponAR::StopFire()
{
    ServerStopFire();
}

void AWeaponAR::MultiCastShot_Implementation()
{
    const FVector Location = GetActorLocation();
    UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponData.ShotSound, Location);
    NotifyAmmoUpdate();
    if (const APawn* OwnerPawn = Cast<APawn>(GetOwner()))
    {
        if (OwnerPawn->IsLocallyControlled())
        {
            ApplyCameraShake();
            PlayMuzzleEffect();
        }
    }
}

void AWeaponAR::ServerReload_Implementation()
{
    Reload();
}

void AWeaponAR::MakeShot()
{
    if (bIsReloading || WeaponData.CurrentAmmo <= 0)
    {
        StopFire();
        return;
    }
    
    WeaponData.CurrentAmmo--;
    
    MultiCastShot();
    
    AActor* MyOwner = GetOwner();
    if (!MyOwner) return;
    
    APlayerController* PC = Cast<APlayerController>(MyOwner->GetInstigatorController());
    if (!PC) return;

    FVector ViewLocation;
    FRotator ViewRotation;
    PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const float HalfRad = FMath::DegreesToRadians(WeaponData.BulletSpread * 0.5f);
    FVector ShotDirection = FMath::VRandCone(ViewRotation.Vector(), HalfRad);
    FVector TraceEnd = ViewLocation + ShotDirection * WeaponData.TraceMaxDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(MyOwner);
    Params.AddIgnoredActor(this);
    Params.bReturnPhysicalMaterial = true;

    const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, TraceEnd, ECC_Visibility, Params);

    if (!bHit) return;

    // TODO: VFX
    DrawDebugSphere(GetWorld(), Hit.ImpactPoint,6.0f,12, FColor::Yellow);

    AActor* HitActor = Hit.GetActor();
    if (HitActor && HitActor->IsA(ACharacterZombie::StaticClass()))
    {
        UGameplayStatics::ApplyPointDamage(
            HitActor,
            40.0f,
            GetActorForwardVector(),
            Hit,
            PC,
            this,
            UDamageType::StaticClass());
    }
}


void AWeaponAR::InitializeWeaponData(FWeaponData* InData)
{
    WeaponData = *InData;
    WeaponData.CurrentAmmo = WeaponData.MagazineCapacity;
}

void AWeaponAR::NotifyAmmoUpdate()
{
    Super::NotifyAmmoUpdate();

    const ACharacterPlayer* OwnerCharacter = Cast<ACharacterPlayer>(GetOwner());
    if (!OwnerCharacter) return;

    const APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;

    if (AOBHUD* HUD = Cast<AOBHUD>(PC->GetHUD()))
    {
        HUD->DisplayAmmo(WeaponData.CurrentAmmo, WeaponData.TotalAmmo);
    }
}

void AWeaponAR::ServerStopFire_Implementation()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AWeaponAR::ServerStartFire_Implementation()
{
    if (bIsReloading || WeaponData.CurrentAmmo <= 0)
        return;

    MakeShot();
    
    GetWorldTimerManager().SetTimer(
        TimerHandle_TimeBetweenShots,
        this,
        &AWeaponAR::MakeShot,
        WeaponData.FireFrequency,
        true
    );
}

void AWeaponAR::PlayMuzzleEffect()
{
    UNiagaraFunctionLibrary::SpawnSystemAttached(
        NiagaraMuzzleFlash, 
        WeaponMesh,                     
        TEXT("Muzzle_AR"),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTarget,
        true
    );
}


