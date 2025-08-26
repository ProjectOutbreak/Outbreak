// Fill out your copyright notice in the Description page of Project Settings.


#include "TimeManager.h"

ATimeManager::ATimeManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentTimeInSeconds = 12 * 3600;
}

void ATimeManager::BeginPlay()
{
	Super::BeginPlay();

	if (bUseRealWorldTime)
	{
		FDateTime Now = FDateTime::UtcNow();
		float RealTimeHour = Now.GetHour() + TimezoneOffset;
		CurrentTimeInSeconds = RealTimeHour * 3600 + Now.GetMinute() * 60 + Now.GetSecond();

		if (CurrentTimeInSeconds >= SecondsInDay)
		{
			CurrentTimeInSeconds = FMath::Fmod(CurrentTimeInSeconds, SecondsInDay);
		}
	}
}

void ATimeManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentTimeInSeconds += DeltaTime * GameTimeSpeed;
	if (CurrentTimeInSeconds >= SecondsInDay)
	{
		CurrentTimeInSeconds = FMath::Fmod(CurrentTimeInSeconds, SecondsInDay);
	}
}

float ATimeManager::GetTimeOfDayInHours() const
{
	return CurrentTimeInSeconds / 3600.f;
}

FRotator ATimeManager::GetSunRotation() const
{
	const float DayRatio = CurrentTimeInSeconds / SecondsInDay;
    const float PitchAngle = 90.0f - (DayRatio * 360.0f);
	const float YawAngle = 180.f;
	if (GEngine)
	{
		FString DebugMessage = FString::Printf(TEXT("Current Time: %.2f, Pitch Angle: %.2f"), GetTimeOfDayInHours(), PitchAngle);
		GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, DebugMessage);
	}

	return FRotator(PitchAngle, YawAngle, 0.f);
}


