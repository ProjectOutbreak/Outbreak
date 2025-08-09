#include "OBGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UOBGameInstance::Init()
{
	Super::Init();

	const FSoftClassPath UIPath(TEXT("/Game/Blueprints/UI/WBP_OBWidget.WBP_OBWidget_C"));
	OBWidgetClass = TSoftClassPtr<UUserWidget>(UIPath);
	
	GraphicsQuality = 2; 
	UE_LOG(LogTemp, Warning, TEXT("GameInstance 초기화 완료"));
}

void UOBGameInstance::BeginLoading()
{
	TArray<FSoftObjectPath> Paths;

	if (OBWidgetClass.ToSoftObjectPath().IsValid())
	{
		Paths.AddUnique(OBWidgetClass.ToSoftObjectPath());
	}
	if (!CachedWidgetClass)
	{
		CachedWidgetClass = OBWidgetClass.LoadSynchronous();
	}
	
	for (const auto& Asset : AssetsToPreload)
	{
		Paths.AddUnique(Asset.ToSoftObjectPath());
	}

	if (Paths.Num() > 0)
	{
		StreamableManager.RequestAsyncLoad(
			Paths,
			FStreamableDelegate::CreateUObject(this, &UOBGameInstance::OnAssetsLoaded)
			);
	}
	else
	{
		OnAssetsLoaded();
	}
}

void UOBGameInstance::OnAssetsLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("모든 에셋 로딩 완료!"));
	UGameplayStatics::OpenLevel(GetWorld(), "L_FirstPhase");
}


