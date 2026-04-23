// Fill out your copyright notice in the Description page of Project Settings.

#include "TimeManager.h"
#include "Data/OutbreakDeveloperSettings.h"
#include "Outbreak/Game/Framework/OutBreakGameInstance.h"

ATimeManager::ATimeManager()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentTimeInSeconds = 12 * 3600;
}

void ATimeManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UOutbreakDeveloperSettings* DevSettings = UOutbreakDeveloperSettings::Get())
	{
		ApplyTimePreset(DevSettings->DefaultTimePreset);
	}
	else if (bUseRealWorldTime)
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
void ATimeManager::SetTimeOfDayHours(float Hour)
{
	const float WrappedHour = FMath::Fmod(FMath::Max(Hour, 0.f), 24.f);
	CurrentTimeInSeconds = WrappedHour * 3600.f;	
}

void ATimeManager::ApplyTimePreset(ETimePreset Preset)
{
	switch (Preset)
	{
		case ETimePreset::Dawn:
			SetTimeOfDayHours(6.f);
			break;
		case ETimePreset::Noon:
			SetTimeOfDayHours(12.f);
			break;
		case ETimePreset::Dusk:
			SetTimeOfDayHours(18.f);
			break;
		case ETimePreset::Midnight:
			SetTimeOfDayHours(0.f);
			break;
		case ETimePreset::CurrentTime:
			{
				const FDateTime Now = FDateTime::UtcNow();
				const float HourF = FMath::Fmod((Now.GetHour() + TimezoneOffset), 24.f) + (Now.GetMinute() / 60.f) + (Now.GetSecond() / 3600.f);
				SetTimeOfDayHours(HourF);
				break;
			}
		case ETimePreset::RandomOne:
			{
				static const ETimePreset Pool[] =
					{ ETimePreset::Dawn, ETimePreset::Noon,
					ETimePreset::Dusk, ETimePreset::Midnight,
					ETimePreset::CurrentTime
					};
				const int32 Index = FMath::RandRange(0, UE_ARRAY_COUNT(Pool) - 1);
				ApplyTimePreset(Pool[Index]);
				break;
			}
		default:
			break;		
	}
}

void ATimeManager::ApplyPresetFromGameInstance()
{
	if (UWorld* World = GetWorld())
	{
		if (const UOutbreakGameInstance* OB = World->GetGameInstance<UOutbreakGameInstance>())
		{
			ApplyTimePreset(OB->GetSelectedTimePreset());
		}
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


