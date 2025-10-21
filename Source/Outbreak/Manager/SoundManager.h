// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Outbreak/Game/Framework/OutBreakGameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundManager.generated.h"


UCLASS()
class OUTBREAK_API USoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	//-----Function-----//
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION() void StartMainBgmShuffle(bool bRestartIfPlaying = false, float FadeInTimme = 0.5f);
	void PlayFootStepSound(EPhysicalSurface InSurfaceType, FVector InLocation);
	
private:
	void PlayNextBgm(float FadeInTime);
	void PlayBgmInternal(USoundBase* Sound, float FadeInTime);

	UFUNCTION() void OnMusicFinished();

	//-----Variables-----//

public:
	UPROPERTY() UAudioComponent* BgmComponent = nullptr;

private:
	float BgmVolume = 0.5f;
	int32 LastBgmIndex = INDEX_NONE;

	UPROPERTY() TObjectPtr<UOutbreakGameInstance> OBGameInstance;
};
