// Fill out your copyright notice in the Description page of Project Settings.


#include "SkyController.h"

#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h" 
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Manager/TimeManager.h"

ASkyController::ASkyController()
{
	PrimaryActorTick.bCanEverTick = true;

	SunMaxIntensity = 0.0f;
	MoonMaxIntensity = 0.0f;
	SkylightMaxIntensity = 0.0f;
}

void ASkyController::BeginPlay()
{
	Super::BeginPlay();
	TimeManagerRef = Cast<ATimeManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATimeManager::StaticClass()));

	GetLightsComponent();
}

void ASkyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (SunLight && MoonLight && SkylightRef && MoonActor && TimeManagerRef)
	{
        const FRotator SunRotation = TimeManagerRef->GetSunRotation();
        SunLight->SetActorRotation(SunRotation);

		const FRotator MoonRotation = ComputeMoonRotation(SunRotation);
	    MoonLight->SetActorRotation(MoonRotation);
		
		const float CurrentHour = TimeManagerRef->GetTimeOfDayInHours();
		float SunAlpha = 0.f, MoonAlpha = 0.f;
		
		ComputeLightAlphas(CurrentHour, SunAlpha, MoonAlpha);
		ApplyLights(SunAlpha, MoonAlpha);
		ApplySkylights(SunAlpha, MoonAlpha);
		ApplyStarSky(CurrentHour);
		UpdateMoonPosition(MoonRotation);
	}
}

void ASkyController::GetLightsComponent()
{
	if (SunLight && SunLight->GetLightComponent())
	{
		SunMaxIntensity = SunLight->GetLightComponent()->Intensity;
	}
	if (MoonLight && MoonLight->GetLightComponent())
	{
		MoonMaxIntensity = MoonLight->GetLightComponent()->Intensity;
	}
	if (SkylightRef && SkylightRef->GetLightComponent())
	{
		SkylightMaxIntensity = SkylightRef->GetLightComponent()->Intensity;
	}
}


FRotator ASkyController::ComputeMoonRotation(const FRotator& SunRotation) const
{
	FRotator MoonRotation = SunRotation;
	MoonRotation.Yaw += 180.0f;
	MoonRotation.Pitch *= -1.0f;
	return MoonRotation;
}


void ASkyController::UpdateMoonPosition(const FRotator& MoonRotation) const
{
	if (!MoonActor) return;
	
	const APlayerCameraManager* CamMgr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FVector Anchor = CamMgr ? CamMgr->GetCameraLocation() : FVector::ZeroVector;
	const FVector Dir = -FRotationMatrix(MoonRotation).GetUnitAxis(EAxis::X);
	const FVector MoonLoc = Anchor + (Dir * MoonRadius);
	const FRotator FaceCamera = (Anchor - MoonLoc).Rotation();
			
	MoonActor->SetActorLocation(MoonLoc);
	MoonActor->SetActorRotation(FaceCamera);
}

void ASkyController::ComputeLightAlphas(float CurrentHour, float& SunAlpha, float& MoonAlpha) const
{
	SunAlpha = 0.0f;
	if (CurrentHour >= 4.f && CurrentHour <= 20.f)
	{
		if (CurrentHour < 7.f) { SunAlpha = FMath::GetMappedRangeValueClamped(FVector2D(4.f, 7.f), FVector2D(0.f, 1.f), CurrentHour); }
		else if (CurrentHour > 17.f) { SunAlpha = FMath::GetMappedRangeValueClamped(FVector2D(17.f, 20.f), FVector2D(1.f, 0.f), CurrentHour); }
		else { SunAlpha = 1.0f; }
	}
          
	MoonAlpha = 0.0f;
	if (CurrentHour > 17.f || CurrentHour < 8.f)
	{
		if (CurrentHour > 17.f && CurrentHour < 20.f) { MoonAlpha = FMath::GetMappedRangeValueClamped(FVector2D(17.f, 20.f), FVector2D(0.f, 1.f), CurrentHour); }
		else if (CurrentHour > 5.f && CurrentHour < 8.f) { MoonAlpha = FMath::GetMappedRangeValueClamped(FVector2D(5.f, 8.f), FVector2D(1.f, 0.f), CurrentHour); }
		else { MoonAlpha = 1.0f; }
	}
}

void ASkyController::ApplyStarSky(float CurrentHour)
{
	if (!NightStarSky) return;
	const bool bShowStars = (CurrentHour < 6.f || CurrentHour >= 18.f);
	NightStarSky->SetActorHiddenInGame(!bShowStars);
}

void ASkyController::ApplyLights(float SunAlpha, float MoonAlpha)
{
	ULightComponent* SunLC = SunLight ? SunLight->GetLightComponent() : nullptr;
	ULightComponent* MoonLC = MoonLight ? MoonLight->GetLightComponent() : nullptr;
	if (!SunLC || !MoonLC) return;

	SunLC->SetIntensity(SunMaxIntensity * SunAlpha);
	MoonLC->SetIntensity(MoonMaxIntensity * MoonAlpha);
	SunLC->SetVisibility(SunAlpha > 0.001f);
	MoonLC->SetVisibility(MoonAlpha > 0.001f);
}

void ASkyController::ApplySkylights(float SunAlpha, float MoonAlpha)
{
	USkyLightComponent* SkyLC = SkylightRef ? SkylightRef->GetLightComponent() : nullptr;
	if (!SkyLC) return;

	const float MinNightLightLevel = 0.15f;
	const float MoonSkylightContribution = 0.4f;

	const float sunA = SunAlpha;
	const float moonA = MoonAlpha * MoonSkylightContribution;
	const float combined = FMath::Clamp(sunA + moonA, 0.f, 1.f);
	const float finalA = FMath::Max(combined, MinNightLightLevel);

	SkyLC->SetIntensity(SkylightMaxIntensity * finalA);
}


