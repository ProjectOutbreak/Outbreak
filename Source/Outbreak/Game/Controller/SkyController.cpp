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

void ASkyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (SunLight && MoonLight && SkylightRef && MoonActor && TimeManagerRef)
    {
       FRotator SunRotation = TimeManagerRef->GetSunRotation();
       SunLight->SetActorRotation(SunRotation);
	   FRotator MoonRotation = SunRotation;
	   MoonRotation.Yaw += 180.0f;
	   MoonRotation.Pitch *= -1.0f;
	   MoonLight->SetActorRotation(MoonRotation);

		if (MoonActor)
		{
			const APlayerCameraManager* CamMgr = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			const FVector Anchor = CamMgr ? CamMgr->GetCameraLocation() : FVector::ZeroVector;
			const FVector Dir = -FRotationMatrix(MoonRotation).GetUnitAxis(EAxis::X);
			const FVector MoonLoc = Anchor + (Dir * MoonRadius);
			const FRotator FaceCamera = (Anchor - MoonLoc).Rotation();
			
			MoonActor->SetActorLocation(MoonLoc);
			MoonActor->SetActorRotation(FaceCamera);
		}

       ULightComponent* SunLightComponent = SunLight->GetLightComponent();
       ULightComponent* MoonLightComponent = MoonLight->GetLightComponent();
       USkyLightComponent* SkylightComponent = SkylightRef->GetLightComponent();
       
       if (SunLightComponent && MoonLightComponent && SkylightComponent)
       {
          const float CurrentHour = TimeManagerRef->GetTimeOfDayInHours();

          float SunAlpha = 0.0f;
          if (CurrentHour >= 4.f && CurrentHour <= 20.f)
          {
              if (CurrentHour < 7.f) { SunAlpha = FMath::GetMappedRangeValueClamped(FVector2D(4.f, 7.f), FVector2D(0.f, 1.f), CurrentHour); }
              else if (CurrentHour > 17.f) { SunAlpha = FMath::GetMappedRangeValueClamped(FVector2D(17.f, 20.f), FVector2D(1.f, 0.f), CurrentHour); }
              else { SunAlpha = 1.0f; }
          }
          
          float MoonAlpha = 0.0f;
          if (CurrentHour > 17.f || CurrentHour < 8.f)
          {
              if (CurrentHour > 17.f && CurrentHour < 20.f) { MoonAlpha = FMath::GetMappedRangeValueClamped(FVector2D(17.f, 20.f), FVector2D(0.f, 1.f), CurrentHour); }
              else if (CurrentHour > 5.f && CurrentHour < 8.f) { MoonAlpha = FMath::GetMappedRangeValueClamped(FVector2D(5.f, 8.f), FVector2D(1.f, 0.f), CurrentHour); }
              else { MoonAlpha = 1.0f; }
          }

          SunLightComponent->SetIntensity(SunMaxIntensity * SunAlpha);
          MoonLightComponent->SetIntensity(MoonMaxIntensity * MoonAlpha);
          SunLightComponent->SetVisibility(SunAlpha > 0.001f);
          MoonLightComponent->SetVisibility(MoonAlpha > 0.001f);

       	  const bool bShowStars = (CurrentHour < 6.f || CurrentHour >= 18.f);
       	  if (NightStarSky)
       	  {
       	  	  NightStarSky->SetActorHiddenInGame(!bShowStars);
       	  }
       	
          const float MinNightLightLevel = 0.15f;

          float SunSkylightAlpha = SunAlpha;
       	  const float MoonSkylightContribution = 0.4f;
          float MoonSkylightAlpha = MoonAlpha * MoonSkylightContribution;
          float CombinedLightAlpha = FMath::Clamp(SunSkylightAlpha + MoonSkylightAlpha, 0.0f, 1.0f);
          float FinalSkylightAlpha = FMath::Max(CombinedLightAlpha, MinNightLightLevel);
          
          float NewSkylightIntensity = SkylightMaxIntensity * FinalSkylightAlpha;
          SkylightComponent->SetIntensity(NewSkylightIntensity);

       	  
       }
    }
}
