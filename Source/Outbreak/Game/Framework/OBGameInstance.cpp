#include "OBGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"
#include "Outbreak/Manager/TimeManager.h"
#include "Sound/SoundCue.h"
#include "Outbreak/Util/AsynchronousLoadingHelper.h"

void UOBGameInstance::Init()
{
	Super::Init();
	
	AddAssetsPath();
	UGraphicsSettingsLibrary::ApplyDefaultGraphics();
	UE_LOG(LogTemp, Warning, TEXT("GameInstance 초기화 완료"));
}

void UOBGameInstance::AddAssetsPath()
{
	ClassesToPreload.Add(TSoftObjectPtr<UClass>(FSoftObjectPath("/Game/Blueprints/UI/WBP_OBWidget.WBP_OBWidget_C")));
	SoundsToPreload.Add(TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/BGM/S_BGM1.S_BGM1")));
	SoundsToPreload.Add(TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/BGM/S_BGM2.S_BGM2")));
	SoundsToPreload.Add(TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/BGM/S_BGM3.S_BGM3")));
	SoundsToPreload.Add(TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/BGM/S_BGM4.S_BGM4")));
	SoundsToPreload.Add(TSoftObjectPtr<USoundBase>(FSoftObjectPath("/Game/Audio/BGM/S_BGM5.S_BGM5")));
}

void UOBGameInstance::BeginLoading()
{
	TArray<FSoftObjectPath> Paths;

	AsynchronousLoadingHelper::AppendPaths(ClassesToPreload, Paths);
	AsynchronousLoadingHelper::AppendPaths(SoundsToPreload, Paths);

	UDataTable* FootstepDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_FootStepSound.DT_FootStepSound"));
	if (FootstepDataTable)
	{
		TArray<FName> RowNames = FootstepDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			const FFootStepSounds* Row = FootstepDataTable->FindRow<FFootStepSounds>(RowName, TEXT(""));
			if (Row)
			{
				if (!Row->GrassSound.IsNull()) Paths.AddUnique(Row->GrassSound.ToSoftObjectPath());
				if (!Row->SandSound.IsNull()) Paths.AddUnique(Row->SandSound.ToSoftObjectPath());
				if (!Row->WoodSound.IsNull()) Paths.AddUnique(Row->WoodSound.ToSoftObjectPath());
				if (!Row->ConcreteSound.IsNull()) Paths.AddUnique(Row->ConcreteSound.ToSoftObjectPath());
				if (!Row->WaterSound.IsNull()) Paths.AddUnique(Row->WaterSound.ToSoftObjectPath());
				if (!Row->EtcSound.IsNull()) Paths.AddUnique(Row->EtcSound.ToSoftObjectPath());
			}
		}
	}
	
	if (Paths.Num() > 0)
	{
		StreamableManager.RequestAsyncLoad(
			Paths,
			FStreamableDelegate::CreateUObject(this, &UOBGameInstance::OnAssetsLoaded)
			);
	}
	else
		OnAssetsLoaded();
}

void UOBGameInstance::OnAssetsLoaded()
{
    UClass* WidgetClass = nullptr;
	for (const auto& ClassPtr : ClassesToPreload)
    {
		if (auto* C = ClassPtr.Get())
		{
			WidgetClass = C; break;
		}
    }	
    CachedWidgetClass = WidgetClass;

	for (const auto& SoundPtr : SoundsToPreload)
	{
		if (auto* S = SoundPtr.Get())
		{
			CachedBgmSounds.Add(S);
		}
	}
	
	UDataTable* FootstepDataTable = FindObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_FootStepSound.DT_FootStepSound"));
	if (FootstepDataTable)
	{
		auto CacheSoundForSurface = [&](EPhysicalSurface SurfaceType, const TSoftObjectPtr<USoundCue>& SoundCuePtr)
		{
			if (USoundBase* Sound = SoundCuePtr.Get())
			{
				CachedFootstepSounds.Add(SurfaceType, Sound);
			}
		};

		TArray<FName> RowNames = FootstepDataTable->GetRowNames();
		for (const FName& RowName : RowNames)
		{
			const FFootStepSounds* Row = FootstepDataTable->FindRow<FFootStepSounds>(RowName, TEXT(""));
			if (Row)
			{
				CacheSoundForSurface(SurfaceType4, Row->GrassSound);
				CacheSoundForSurface(SurfaceType5, Row->SandSound);
				CacheSoundForSurface(SurfaceType6, Row->WoodSound);
				CacheSoundForSurface(SurfaceType7, Row->ConcreteSound);
				CacheSoundForSurface(SurfaceType8, Row->WaterSound);
				CacheSoundForSurface(SurfaceType9, Row->EtcSound);
			}
		}
	}
    UE_LOG(LogTemp, Warning, TEXT("모든 에셋 로딩 완료!"));
	UGameplayStatics::OpenLevel(GetWorld(), "L_FirstPhase");
}

void UOBGameInstance::ApplySelectedTimePreset()
{
	UWorld* World = GetWorld();

	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(World, ATimeManager::StaticClass(), FoundManagers);

	ATimeManager* TM = Cast<ATimeManager>(FoundManagers[0]);
	TM->ApplyPresetFromGameInstance();
}

