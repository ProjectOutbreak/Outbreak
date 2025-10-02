// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outbreak/Util/Define.h"
#include "TimeManager.generated.h"

UCLASS()
class OUTBREAK_API ATimeManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATimeManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	//-----Variables-----//

	UPROPERTY(EditAnywhere, Category= "Time Of Day")
	float GameTimeSpeed = 60.f; // 1.0 = 실제 시간

	UPROPERTY(EditAnywhere, Category= "Time Of Day")
	bool bUseRealWorldTime = false;
	
	UPROPERTY()
	float CurrentTimeInSeconds;

	UPROPERTY()
	float TimezoneOffset = 9.0f;


	//-----Function-----//

	UFUNCTION()
	void SetTimeOfDayHours(float Hour);

	UFUNCTION()
	void ApplyTimePreset(ETimePreset Preset);

	UFUNCTION()
	void ApplyPresetFromGameInstance();
	
	UFUNCTION()
	float GetTimeOfDayInHours() const;

	UFUNCTION()
	FRotator GetSunRotation() const;


public:
	const float SecondsInDay = 86400.f;
};
