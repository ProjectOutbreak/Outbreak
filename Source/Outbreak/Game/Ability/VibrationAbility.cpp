#include "VibrationAbility.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Util/CameraShake.h"

UVibrationAbility::UVibrationAbility()
{
	AbilityType = EAbilityType::Vibration;
	
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> VibrationEffectRef(TEXT("/Game/Art/VFX/Niagara/NS_VibrationSmoke.NS_VibrationSmoke"));
	if (VibrationEffectRef.Succeeded())
	{
		VibrationEffectAsset = VibrationEffectRef.Object;
	}
}

void UVibrationAbility::OnEquip()
{
	GetOwner()->GetWorldTimerManager().SetTimer(VibrationTimerHandle,this, &UVibrationAbility::OnVibrationAbility,VibrationInterval,true);
}

void UVibrationAbility::OnUnequip()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(VibrationTimerHandle);
}

void UVibrationAbility::OnVibrationAbility()
{
	SpawnVibrationEffect();
	
	const FVector Origin = GetOwner()->GetActorLocation();
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetOwner()->GetWorld(), ACharacterPlayer::StaticClass(), Actors);

	for (AActor* Actor : Actors)
	{
		ACharacterPlayer* Player = Cast<ACharacterPlayer>(Actor);
		if (!Player || Player == GetOwner()) continue;

		const float Distance = FVector::Dist(Player->GetActorLocation(), Origin);
		if (Distance <= VibrationRange)
		{
			UGameplayStatics::ApplyDamage(Player, VibrationDamage, GetOwner()->GetController(), GetOwner(), UDamageType::StaticClass());

			if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
			{
				PC->ClientStartCameraShake(UCameraShake::StaticClass(), VibrationIntensity);
			}
		}
	}
}

void UVibrationAbility::SpawnVibrationEffect()
{
	if (!VibrationEffectAsset || !GetOwner()) return;
	
	FVector SpawnLocation = GetOwner()->GetActorLocation();
    
	if (const USkeletalMeshComponent* Mesh = GetOwner()->GetMesh())
	{
		SpawnLocation = Mesh->GetSocketLocation(TEXT("ik_foot_root"));
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetOwner()->GetWorld(), VibrationEffectAsset, SpawnLocation);
}