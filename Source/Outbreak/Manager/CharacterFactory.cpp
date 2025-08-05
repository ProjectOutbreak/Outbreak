// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterFactory.h"
#include "Outbreak/Character/Zombie/SpecialZombie/GymRatZombie.h"

UCharacterFactory::UCharacterFactory()
{
    
}

bool UCharacterFactory::SpawnCharacter(const TSubclassOf<AActor>& SpawnClass, const FTransform& SpawnTransform, const FActorSpawnParameters& SpawnParams)
{
    const AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnTransform, SpawnParams);
    if (!SpawnedActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] Failed Spawn Character : %s"), CURRENT_CONTEXT, *SpawnClass->GetName());

        return false;
    }
    
    return true;
}