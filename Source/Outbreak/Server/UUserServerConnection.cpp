#include "UUserServerConnection.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "Common/TcpSocketBuilder.h"
#include "HAL/RunnableThread.h"
#include "Containers/StringConv.h"
#include "Async/Async.h"

FSocketReceiverRunnable::FSocketReceiverRunnable(FSocket* InSocket, TQueue<FString, EQueueMode::Mpsc>* InMessageQueue)
	: Socket(InSocket)
	, MessageQueue(InMessageQueue)
{
}
bool FSocketReceiverRunnable::Init()
{
	bIsRunning = true;
	return true;
}

uint32 FSocketReceiverRunnable::Run()
{
	// main loop
	while (bIsRunning)
	{
		if (!Socket || Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
		{
			FPlatformProcess::Sleep(0.1f);
			continue;
		}

		uint32 PendingDataSize = 0;
		if (Socket->HasPendingData(PendingDataSize) && PendingDataSize > 0)
		{
			TArray<uint8> Buffer;
			Buffer.SetNumUninitialized(PendingDataSize);

			int32 BytesRead = 0;
			Socket->Recv(Buffer.GetData(), Buffer.Num(), BytesRead);
            
			FString ReceivedData = Remainder + FString(UTF8_TO_TCHAR(Buffer.GetData()));
			Remainder.Empty();

			// 서버가 메시지를 \n 기준으로 보내므로, \n 기준으로 메시지를 분리
			TArray<FString> Lines;
			ReceivedData.ParseIntoArrayLines(Lines, false);

			// 만약 데이터가 \n으로 끝나지 않았다면, 마지막 부분은 다음 수신을 위해 Remainder에 저장
			if (!ReceivedData.EndsWith(TEXT("\n")))
			{
				Remainder = Lines.Pop();
			}
			
			for (const FString& Line : Lines)
			{
				MessageQueue->Enqueue(Line);
			}
		}
		else
		{
			// 처리할 데이터가 없으면 잠시 대기하여
			FPlatformProcess::Sleep(0.01f);
		}
	}
	return 0;
}

void FSocketReceiverRunnable::Stop()
{
	bIsRunning = false;
}

void FSocketReceiverRunnable::Exit()
{

}

void UUserServerConnection::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // 0.1초마다 메시지를 확인하는 타이머
    GetGameInstance()->GetTimerManager().SetTimer(MessageQueueTimerHandle, this, &UUserServerConnection::ProcessMessageQueue, 0.1f, true);
}

void UUserServerConnection::Deinitialize()
{
    DisconnectFromServer();
    GetGameInstance()->GetTimerManager().ClearTimer(MessageQueueTimerHandle);
    Super::Deinitialize();
}

void UUserServerConnection::ConnectToServer(const FString& IPAddress, int32 Port)
{
    if (IsConnected())
    {
        UE_LOG(LogTemp, Warning, TEXT("SocketSubsystem: Already connected."));
        return;
    }
    
    // 주소 파싱
    FIPv4Address IP;
    if (!FIPv4Address::Parse(IPAddress, IP))
    {
        UE_LOG(LogTemp, Error, TEXT("SocketSubsystem: Invalid IP address format."));
        OnConnectionStateChanged.Broadcast(false);
        return;
    }

    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Addr->SetIp(IP.Value);
    Addr->SetPort(Port);

    // 소켓 생성
    Socket = FTcpSocketBuilder(TEXT("GameClientSocket")).AsBlocking().Build();
    if (!Socket)
    {
        UE_LOG(LogTemp, Error, TEXT("SocketSubsystem: Failed to create socket."));
        OnConnectionStateChanged.Broadcast(false);
        return;
    }

    // 연결 시도
    if (Socket->Connect(*Addr))
    {
        UE_LOG(LogTemp, Log, TEXT("SocketSubsystem: Connection successful."));

        // 수신 스레드 시작
        ReceiverRunnable = new FSocketReceiverRunnable(Socket, &MessageQueue);
        ReceiverThread = FRunnableThread::Create(ReceiverRunnable, TEXT("SocketReceiverThread"));
        
        OnConnectionStateChanged.Broadcast(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SocketSubsystem: Failed to connect to server."));
        DisconnectFromServer(); 
    }
}

void UUserServerConnection::DisconnectFromServer()
{
    if (ReceiverThread)
    {
        ReceiverRunnable->Stop();
        ReceiverThread->WaitForCompletion();
        delete ReceiverThread;
        ReceiverThread = nullptr;
        delete ReceiverRunnable;
        ReceiverRunnable = nullptr;
    }

    if (Socket)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
        Socket = nullptr;
    }
    
    // 연결 종료 이벤트는 이미 연결된 상태였을 때만 호출
    if (OnConnectionStateChanged.IsBound() && IsConnected())
    {
       OnConnectionStateChanged.Broadcast(false);
    }
    UE_LOG(LogTemp, Log, TEXT("SocketSubsystem: Disconnected."));
}

bool UUserServerConnection::SendMessage(const FString& Message)
{
    if (!IsConnected())
    {
        UE_LOG(LogTemp, Warning, TEXT("SocketSubsystem: Not connected. Cannot send message."));
        return false;
    }

    FString FormattedMessage = Message + TEXT("\n");
    FTCHARToUTF8 Converter(*FormattedMessage);
    int32 BytesSent = 0;
    
    bool bSuccess = Socket->Send(reinterpret_cast<const uint8*>(Converter.Get()), Converter.Length(), BytesSent);
    if (!bSuccess)
    {
        UE_LOG(LogTemp, Error, TEXT("SocketSubsystem: Failed to send message."));
    }
    return bSuccess;
}

bool UUserServerConnection::IsConnected() const
{
    return Socket != nullptr && Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected;
}

void UUserServerConnection::ProcessMessageQueue()
{
    while (!MessageQueue.IsEmpty())
    {
        FString Message;
        if (MessageQueue.Dequeue(Message))
        {
            UE_LOG(LogTemp, Log, TEXT("SocketSubsystem: Message Received: %s"), *Message);
            OnMessageReceived.Broadcast(Message);
        }
    }
}


UUserServerConnection::UUserServerConnection()
{
}

UUserServerConnection::~UUserServerConnection()
{
}
