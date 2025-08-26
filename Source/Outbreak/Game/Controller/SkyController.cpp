// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyController.h"

#include "Components/LightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Manager/TimeManager.h"

ASkyController::ASkyController()
{
	PrimaryActorTick.bCanEverTick = true;

	SunMaxIntensity = 0.0f;
	MoonMaxIntensity = 0.0f;
}

void ASkyController::BeginPlay()
{
	Super::BeginPlay();
	TimeManagerRef = Cast<ATimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimeManager::StaticClass()));

	if (SunLight && SunLight->GetLightComponent())
	{
		SunMaxIntensity = SunLight->GetLightComponent()->Intensity;
	}
	if (MoonLight && MoonLight->GetLightComponent())
	{
		MoonMaxIntensity = MoonLight->GetLightComponent()->Intensity;
	}
}

void ASkyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SunLight && MoonLight &&TimeManagerRef)
	{
		FRotator SunRotation = TimeManagerRef->GetSunRotation();
		SunLight->SetActorRotation(SunRotation);

		FRotator MoonRotation = SunRotation;
		MoonRotation.Yaw += 180.0f;
		MoonRotation.Pitch *= -1.0f;
		MoonLight->SetActorRotation(MoonRotation);

		ULightComponent* SunLightComponent = SunLight->GetLightComponent();
		ULightComponent* MoonLightComponent = MoonLight->GetLightComponent();
		if (SunLightComponent && MoonLightComponent)
		{
			const float SunDirectionZ = SunLight->GetActorForwardVector().Z;
			const float TransitionAlpha = FMath::Clamp(-SunDirectionZ, 0.0f, 1.0f);
			
			SunLightComponent->SetIntensity(SunMaxIntensity * TransitionAlpha);
			MoonLightComponent->SetIntensity(MoonMaxIntensity * (1.0f - TransitionAlpha));

			SunLightComponent->SetVisibility(SunMaxIntensity > 0.001f);
			MoonLightComponent->SetVisibility(MoonMaxIntensity < 0.999f);
		}		
	}
}
