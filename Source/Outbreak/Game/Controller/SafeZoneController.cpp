// Fill out your copyright notice in the Description page of Project Settings.


#include "SafeZoneController.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "CutsceneController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Outbreak/Game/Framework/InGameMode.h"
#include "Outbreak/Game/Framework/OutBreakGameState.h"
#include "Outbreak/Manager/SoundManager.h"

ASafeZoneController::ASafeZoneController()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	StartSafeZoneCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("StartSafeZone"));
	StartSafeZoneCollision->SetupAttachment(RootComponent);

	EndSafeZoneCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("EndSafeZone"));
	EndSafeZoneCollision->SetupAttachment(RootComponent);
}

void ASafeZoneController::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> FoundWalls;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInvisibleWall::StaticClass(), FoundWalls);
	if (FoundWalls.Num() > 0)
	{
		InvisibleWall = Cast<AInvisibleWall>(FoundWalls[0]);
	}
	if (StartSafeZoneCollision)
	{
		StartSafeZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneController::OnStartZoneEnter);
		UE_LOG(LogTemp, Warning, TEXT("[SafeZone] 시작 존에 진입해서 등록"));

		TArray<AActor*> OverlappingActors;
		StartSafeZoneCollision->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
		for (AActor* Actor : OverlappingActors)
		{
			if (ACharacter* Character = Cast<ACharacter>(Actor))
			{
				PlayersInStartZone.Add(Character);
			}
		}
		
		StartSafeZoneCollision->OnComponentEndOverlap.AddDynamic(this, &ASafeZoneController::OnStartZoneExit);
	}
	if (EndSafeZoneCollision)
	{
		EndSafeZoneCollision->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneController::OnEndZoneEnter);

	}
	InGameModeRef = Cast<AInGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	CutsceneManager = NewObject<UCutsceneController>();
	CutsceneManager->AddToRoot();
	CutsceneManager->Init(GetWorld());
}

void ASafeZoneController::OnEndZoneEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()) return;	

	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		if(Character->GetController() && Character->GetController()->IsPlayerController())
		{
			UE_LOG(LogTemp, Log, TEXT("[SafeZone] 캐릭터 %s 종료 존에 진입"), *Character->GetName());

			PlayersInEndZone.Add(Character);

			int32 TotalPlayers = UGameplayStatics::GetNumPlayerControllers(GetWorld());

			if (PlayersInEndZone.Num() == TotalPlayers)
			{
				if (InGameModeRef && InGameModeRef->IsMatchInProgress())
				{
					if (AOutBreakGameState* GS = GetWorld()->GetGameState<AOutBreakGameState>())
					{
						if (GS->SpawnerInstance)
						{
							GS->SpawnerInstance->Destroy();
							UE_LOG(LogTemp, Warning, TEXT("Spawner is Deleted!"))
						}
					}
					InGameModeRef->EndMatch();
					InGameModeRef->ProceedToNextLevel();
				}
			}
		}		
	}
}

void ASafeZoneController::OnStartZoneEnter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character && !PlayersInStartZone.Contains(Character))
	{
		PlayersInStartZone.Add(Character);
		UE_LOG(LogTemp, Warning, TEXT("[SafeZone] %s 시작 존에 진입해서 등록"), *Character->GetName());
	}
}


void ASafeZoneController::OnStartZoneExit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* Character = Cast<ACharacter>(OtherActor))
	{
		UE_LOG(LogTemp, Log, TEXT("[SafeZone] 캐릭터 %s 시작 존에서 이탈"), *Character->GetName());

		PlayersInStartZone.Remove(Character);
		if (PlayersInStartZone.Num() == 0)
		{
			if (CutsceneManager && !CutsceneManager->bHasPlayedCutscene)
			{
				if (InvisibleWall)
				{
					InvisibleWall->DisableWall();
				}
				CutsceneManager->PlayCutscene(CutsceneSequence);
				CutsceneManager->bHasPlayedCutscene = true;
				StartSafeZoneCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				// hard coded playing BGM
				USoundBase* BGM = LoadObject<USoundBase>(nullptr, TEXT("SoundWave'/Game/Sounds/BGM1.BGM1'"));
				if (BGM)
				{
					// SoundManager에서 BGM 재생

					if (USoundManager* SoundManager = GetGameInstance()->GetSubsystem<USoundManager>())
					{

						SoundManager->PlayPersistentBGM(BGM);
					}

				}
			}
		}
	}
}
