#include "OBGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"
#include "Outbreak/Manager/TimeManager.h"
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

