// Fill out your copyright notice in the Description page of Project Settings.


#include "OutbreakGameLiftSubsystem.h"


void UOutbreakGameLiftSubsystem::OnStartGameSession(Aws::GameLift::Server::Model::GameSession GameSession)
{
#if WITH_GAMELIFT
	UE_LOG(LogTemp, Display, TEXT("GameLift: OnStartGameSession Received!"));
	if (GameLiftSdkModule)
	{
		GameLiftSdkModule->ActivateGameSession();

		UWorld* World = GetWorld();
		if (World)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameLift: Traveling to Gameplay Map..."));
			World->ServerTravel(TEXT("/Game/Maps/L_TestBed_Play?listen"));
		}
		else
        {
             UE_LOG(LogTemp, Error, TEXT("GameLift: World is NULL! Cannot Travel."));
        }
	}
#endif
}

void UOutbreakGameLiftSubsystem::OnProcessTerminate()
{
#if WITH_GAMELIFT
	UE_LOG(LogTemp, Warning, TEXT("GameLift: TERMINATE REQUEST RECEIVED. Shutting down..."));
	GameLiftSdkModule->ProcessEnding();
    
	FGenericPlatformMisc::RequestExit(false);
#endif
}

bool UOutbreakGameLiftSubsystem::OnHealthCheck()
{
	return bIsServerHealthy;
}

void UOutbreakGameLiftSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GameLiftSdkModule = nullptr;
	bIsInitialized = false;
	bIsServerHealthy = true;
	
	OnWorldLoadedDelegateHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UOutbreakGameLiftSubsystem::OnWorldLoaded);
}

void UOutbreakGameLiftSubsystem::Deinitialize()
{
	if (OnWorldLoadedDelegateHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(OnWorldLoadedDelegateHandle);
		OnWorldLoadedDelegateHandle.Reset();
	}
#if WITH_GAMELIFT
	if (GameLiftSdkModule)
	{
		GameLiftSdkModule->ProcessEnding();
	}
#endif
	Super::Deinitialize();
}

void UOutbreakGameLiftSubsystem::StartGameServer()
{
	if (!IsRunningDedicatedServer()) return;
	if (bIsInitialized) return;

#if WITH_GAMELIFT
	UE_LOG(LogTemp,Log,TEXT("[GameLiftSubsystem] Starting Init Sequences..."));

	GameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
	FString SDKVersion = GameLiftSdkModule->GetSdkVersion().GetResult();
	UE_LOG(LogTemp, Display, TEXT(">> Current GameLift SDK Version: %s <<"), *SDKVersion);
	
	UE_LOG(LogTemp, Log, TEXT(">> EC2 MANAGED MODE (Default) <<"));
	FGameLiftGenericOutcome InitOutcome = GameLiftSdkModule->InitSDK();
	if (!InitOutcome.IsSuccess())
	{
		UE_LOG(LogTemp, Error, TEXT("InitSDK Failed: %s"), *InitOutcome.GetError().m_errorMessage);
		return;
	}
	
	FProcessParameters ProcessParams;
	int32 Port = FURL::UrlConfig.DefaultPort;
	FParse::Value(FCommandLine::Get(), TEXT("port="), Port);
	ProcessParams.port = Port;
	TArray<FString> LogFiles;
    LogFiles.Add(TEXT("Outbreak/Saved/Logs/Outbreak.log")); 
    ProcessParams.logParameters = LogFiles;

	ProcessParams.OnStartGameSession.BindUObject(this, &UOutbreakGameLiftSubsystem::OnStartGameSession);
	ProcessParams.OnTerminate.BindUObject(this, &UOutbreakGameLiftSubsystem::OnProcessTerminate);
	ProcessParams.OnHealthCheck.BindUObject(this, &UOutbreakGameLiftSubsystem::OnHealthCheck);

	FGameLiftGenericOutcome ReadyOutcome = GameLiftSdkModule->ProcessReady(ProcessParams);

	if (ReadyOutcome.IsSuccess())
	{
		UE_LOG(LogTemp, Display, TEXT("SUCCESS: ProcessReady! Waiting for GameSession..."));
		bIsInitialized = true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ProcessReady Failed: %s"), *ReadyOutcome.GetError().m_errorMessage);
	}
#endif	
}

void UOutbreakGameLiftSubsystem::EndGameServer()
{
#if WITH_GAMELIFT
	if (GameLiftSdkModule)
	{
		OnProcessTerminate();
	}
#endif
}

void UOutbreakGameLiftSubsystem::OnWorldLoaded(UWorld* World)
{
	if (bIsInitialized) return;

	if (!IsRunningDedicatedServer()) return;

	if (World != GetGameInstance()->GetWorld()) return;
	StartGameServer();
}


void UOutbreakGameLiftSubsystem::TriggerProcessEnding()
{
	if (bProcessEndingInitiated) return;
	bProcessEndingInitiated = true;

#if WITH_GAMELIFT
	UE_LOG(LogTemp,Warning,TEXT("[GameLiftSubsystem] Calling TriggerProcessEnding..."));
	if (GameLiftSdkModule)
	{
		GameLiftSdkModule->ProcessEnding();
	}

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TimerHandle;
		World->GetTimerManager().SetTimer(TimerHandle, [this]()
		{
			UE_LOG(LogTemp, Error, TEXT("[GameLift] Timer Expired. Requesting Exit(false)..."));
			FGenericPlatformMisc::RequestExit(false);
		}, 1.0f, false);
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
#endif
}
