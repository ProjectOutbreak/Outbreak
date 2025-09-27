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
}

void UOBGameInstance::BeginLoading()
{
	TArray<FSoftObjectPath> Paths;

	AsynchronousLoadingHelper::AppendPaths(ClassesToPreload, Paths);
	
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
		if (auto* C = ClassPtr.Get()) { WidgetClass = C; break; }
    }
	
    CachedWidgetClass = WidgetClass;
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

