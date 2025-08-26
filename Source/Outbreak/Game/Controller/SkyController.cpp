// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyController.h"

#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Manager/TimeManager.h"

ASkyController::ASkyController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASkyController::BeginPlay()
{
	Super::BeginPlay();
	TimeManagerRef = Cast<ATimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimeManager::StaticClass()));
}

void ASkyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SunLight && TimeManagerRef)
	{
		FRotator NewSunRotation = TimeManagerRef->GetSunRotation();
		SunLight->SetActorRotation(NewSunRotation);
	}
}
