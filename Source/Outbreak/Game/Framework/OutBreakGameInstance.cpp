#include "OBGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Outbreak/Game/Graphics/GraphicsSettingsLibrary.h"
#include "Outbreak/Manager/TimeManager.h"
#include "Outbreak/Public/Framework/LoadingPlayerController.h"
#include "Outbreak/Util/AsynchronousLoadingHelper.h"
#include "Utilities/DebugHelper.h"

void UOutBreakGameInstance::Init()
{
	Super::Init();
	
	AddAssetsPath();
	UGraphicsSettingsLibrary::ApplyDefaultGraphics();
	UE_LOG(LogTemp, Warning, TEXT("GameInstance 초기화 완료"));
}

void UOutBreakGameInstance::AddAssetsPath()
{
	ClassesToPreload.Add(TSoftObjectPtr<UClass>(FSoftObjectPath("/Game/Blueprints/UI/WBP_OBWidget.WBP_OBWidget_C")));
}

void UOutBreakGameInstance::BeginLoading()
{
	TArray<FSoftObjectPath> Paths;

	AsynchronousLoadingHelper::AppendPaths(ClassesToPreload, Paths);
	
	if (Paths.Num() > 0)
	{
		StreamableManager.RequestAsyncLoad(
			Paths,
			FStreamableDelegate::CreateUObject(this, &UOutBreakGameInstance::OnAssetsLoaded)
			);
	}
	else
		OnAssetsLoaded();
}

void UOutBreakGameInstance::OnAssetsLoaded()
{
    UClass* WidgetClass = nullptr;
	for (const auto& ClassPtr : ClassesToPreload)
    {
		if (auto* C = ClassPtr.Get()) { WidgetClass = C; break; }
    }
	
    CachedWidgetClass = WidgetClass;
	
    if (APlayerController* PC = GetFirstLocalPlayerController(); IsValid(PC))
	{
		if (ALoadingPlayerController* LoadingPc = Cast<ALoadingPlayerController>(PC))
		{
			const FString IsServer = PC->HasAuthority() ? TEXT("[Server]") : TEXT("[Client]");
			const FString DebugMsg = FString::Printf(TEXT("%s Player has completed loading. Notifying GameMode..."), *IsServer);
			PRINT_WITH_CURRENT_CONTEXT(DebugMsg);
			
			LoadingPc->Server_NotifyLoadingComplete();
		}
	}
}

void UOutBreakGameInstance::ApplySelectedTimePreset()
{
	UWorld* World = GetWorld();

	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(World, ATimeManager::StaticClass(), FoundManagers);

	ATimeManager* TM = Cast<ATimeManager>(FoundManagers[0]);
	TM->ApplyPresetFromGameInstance();
}
