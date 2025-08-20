#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HAL/Runnable.h"
#include "HAL/ThreadSafeBool.h"
#include "Containers/Queue.h"
#include "UUserServerConnection.generated.h"

class FSocket;
class FSocketReceiverRunnable;

// Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageReceived, const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionStateChanged, bool, bIsConnected);

UCLASS()
class OUTBREAK_API UUserServerConnection : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:

	// override USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// TCP Connection, DisConnect Method
	void ConnectToServer(const FString& IPAddress = TEXT("127.0.0.1"), int32 Port = 7777);
	void DisconnectFromServer();

	bool SendMessage(const FString& Message);
	bool IsConnected() const;
public:

    UPROPERTY(BlueprintAssignable, Category = "Socket")
	FOnMessageReceived OnMessageReceived;

	// Broadcast if Connection state is changed
	UPROPERTY(BlueprintAssignable, Category = "Socket")
	FOnConnectionStateChanged OnConnectionStateChanged;

private:
	void ProcessMessageQueue();	

	FSocket* Socket;
	FRunnableThread* ReceiverThread = nullptr;
	FSocketReceiverRunnable* ReceiverRunnable = nullptr;
	TQueue<FString, EQueueMode::Mpsc> MessageQueue;
	FTimerHandle MessageQueueTimerHandle;
	
	UUserServerConnection();
	~UUserServerConnection();
};

class FSocketReceiverRunnable : public FRunnable
{
public:
	FSocketReceiverRunnable(FSocket* InSocket, TQueue<FString, EQueueMode::Mpsc>* InMessageQueue);
    
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;
private:
	FThreadSafeBool bIsRunning = false;
	FSocket* Socket = nullptr;
	TQueue<FString, EQueueMode::Mpsc>* MessageQueue = nullptr;
	FString Remainder;
};