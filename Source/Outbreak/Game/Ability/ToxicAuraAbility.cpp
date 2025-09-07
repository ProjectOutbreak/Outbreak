// Fill out your copyright notice in the Description page of Project Settings.

#include "ToxicAuraAbility.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"

UToxicAuraAbility::UToxicAuraAbility()
{
	AbilityType = EAbilityType::ToxicAura;
}

void UToxicAuraAbility::OnEquip()
{
	GetOwner()->GetWorldTimerManager().SetTimer(ToxicAuraTimerHandle, this, &UToxicAuraAbility::OnToxicAuraAbility, CheckInterval,true);
}

void UToxicAuraAbility::OnUnequip()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(ToxicAuraTimerHandle);

	for (AActor* Player : AffectedPlayers)
	{
		if (ACharacterPlayer* CharPlayer = Cast<ACharacterPlayer>(Player))
		{
			CharPlayer->UpdateToxicAuraEffect(0.0f);
		}
	}
	AffectedPlayers.Empty();
}

void UToxicAuraAbility::OnToxicAuraAbility()
{
	if (!Owner) return;

	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Owner->GetActorLocation(), AuraRadius, ObjectTypes, ACharacterPlayer::StaticClass(), {Owner}, OverlappingActors);

	TSet CurrentPlayersInAura(OverlappingActors);
    
	TSet<AActor*> EnteredPlayers = CurrentPlayersInAura.Difference(AffectedPlayers);
	for (AActor* Player : EnteredPlayers)
	{
		if (ACharacterPlayer* PlayerChar = Cast<ACharacterPlayer>(Player))
		{
			PlayerChar->UpdateToxicAuraEffect(0.08f);
		}
	}

	TSet<AActor*> LeftPlayers = AffectedPlayers.Difference(CurrentPlayersInAura);
	for (AActor* Player : LeftPlayers)
	{
		if (ACharacterPlayer* PlayerChar = Cast<ACharacterPlayer>(Player))
		{
			PlayerChar->UpdateToxicAuraEffect(0.0f);
		}
	}
    
	AffectedPlayers = CurrentPlayersInAura;
}
