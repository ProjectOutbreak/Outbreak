// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "Components/AudioComponent.h"
#include "Outbreak/Game/Framework/OBGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void USoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (UWorld* World = GetWorld())
	{
		OBGameInstance = World->GetGameInstance<UOBGameInstance>();
	}
}

void USoundManager::Deinitialize()
{
	if (IsValid(BgmComponent))
	{
		BgmComponent->Stop();
		BgmComponent->DestroyComponent();
		BgmComponent = nullptr;
	}
	Super::Deinitialize();
}

void USoundManager::StartMainBgmShuffle(bool bRestartIfPlaying, float FadeInTime)
{
	if (!GetWorld() || GetWorld()->IsNetMode(NM_DedicatedServer) || !IsValid(OBGameInstance))
		return;

	if (!IsValid(BgmComponent))
	{
		BgmComponent = NewObject<UAudioComponent>(GetWorld());
		BgmComponent->bAutoActivate = false;
		BgmComponent->bIsUISound = false;
		BgmComponent->bAutoActivate = false;
		BgmComponent->OnAudioFinished.AddDynamic(this, &USoundManager::USoundManager::OnMusicFinished);
		BgmComponent->RegisterComponent();
	}

	if (BgmComponent->IsPlaying())
	{
		if (!bRestartIfPlaying) return;
		BgmComponent->Stop();			
	}
	PlayNextBgm(FadeInTime);
}

void USoundManager::OnMusicFinished()
{
	PlayNextBgm(0.2f);
}

void USoundManager::PlayNextBgm(const float FadeInTime)
{
	if (!IsValid(OBGameInstance)) return;
	const TArray<TObjectPtr<USoundBase>>& BgmList = OBGameInstance->GetCachedBgmList();
	
	const int32 N = BgmList.Num();
	if (N <= 0) return;
	int32 NextIdx = 0;
	if (N == 1) NextIdx = 0;
	else
	{
		int32 Candidate = FMath::RandRange(0, N - 1);
		if (Candidate == LastBgmIndex) Candidate = (Candidate + 1) % N;
		NextIdx = Candidate;
	}
	LastBgmIndex = NextIdx;

	USoundBase* SoundToPlay = BgmList[NextIdx];
	PlayBgmInternal(SoundToPlay, FadeInTime);
}

void USoundManager::PlayBgmInternal(USoundBase* Sound, const float FadeInTime)
{
	if (!IsValid(BgmComponent) || !Sound) return;

	BgmComponent->SetSound(Sound);
	BgmComponent->SetVolumeMultiplier(BgmVolume);

	if (FadeInTime > 0.0f)
	{
		BgmComponent->FadeIn(FadeInTime, BgmVolume);
	}
	else
	{
		BgmComponent->Play();
	}
}

void USoundManager::PlayFootStepSound(EPhysicalSurface InSurfaceType, FVector InLocation)
{
	if (!IsValid(OBGameInstance)) return;
	const TMap<EPhysicalSurface, TObjectPtr<USoundBase>>& CachedSounds = OBGameInstance->GetCachedFootStepSounds();
	const TObjectPtr<USoundBase>* FoundSound = CachedSounds.Find(InSurfaceType);
    
	if (FoundSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, *FoundSound, InLocation, 1.f, 1.f);
	}
}
