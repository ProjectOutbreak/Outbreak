// Fill out your copyright notice in the Description page of Project Settings.

#include "SelfDestructAbility.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

USelfDestructAbility::USelfDestructAbility()
{
	AbilityType = EAbilityType::SelfDestruct;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionFX(TEXT("/Game/Art/VFX/Niagara/NS_Explosion_Big_A.NS_Explosion_Big_A"));
	if (ExplosionFX.Succeeded())
	{
		ExplosionEffect = ExplosionFX.Object;
	}
}

void USelfDestructAbility::OnActivate()
{
	const UWorld* World = GetWorld();
	AActor* AbilityOwner = GetOwner();
	
	if (!AbilityOwner || !World || !ExplosionEffect) return;
	
	const FVector ExplosionLocation = AbilityOwner->GetActorLocation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, ExplosionEffect, ExplosionLocation);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(AbilityOwner);

	UGameplayStatics::ApplyRadialDamage(
		World,
		ExplosionDamage,
		ExplosionLocation,
		ExplosionRadius,
		UDamageType::StaticClass(),
		ActorsToIgnore,
		AbilityOwner,
		nullptr,
		true
	);
}

void USelfDestructAbility::OnDeactivate()
{
	
}
