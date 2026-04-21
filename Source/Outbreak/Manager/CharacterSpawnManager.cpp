// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterSpawnManager.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/CollectionHelper.h"
#include "Outbreak/Util/DataTableHelper.h"
#include "Outbreak/Util/EnumHelper.h"

ACharacterSpawnManager::ACharacterSpawnManager()
{
	bReplicates = true;
	AActor::SetReplicateMovement(false);
}

void ACharacterSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	DataTableHelper::LoadDataTableToMap(ZombieDataTable, ZombieDataMap);

	UpdateSettingData();
	UpdateWaveData();
}

void ACharacterSpawnManager::SetSettingId(const FName InSettingId)
{
	if (SpawnerSettingId != InSettingId)
	{
		SpawnerSettingId = InSettingId;
		UpdateSettingData();
	}
}

void ACharacterSpawnManager::SetWaveId(FName InWaveId)
{
	if (WaveId != InWaveId)
	{
		WaveId = InWaveId;
		UpdateWaveData();
	}
}

void ACharacterSpawnManager::Activate()
{
	if (!HasAuthority() || bIsActivated)
		return;
	
	bIsActivated = true;
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&ACharacterSpawnManager::SpawnEnemies,
		SpawnerSettingData.SpawnInterval,
		true
	);
}

void ACharacterSpawnManager::Deactivate()
{
	if (!HasAuthority() || !bIsActivated)
		return;
	
	bIsActivated = false;
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void ACharacterSpawnManager::UpdateSettingData()
{
	FSpawnerSettingData NewSettings;
	if (!DataTableHelper::GetDataFromDataTable(SpawnerSettingDataTable, SpawnerSettingId, NewSettings))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Update Setting Failed : %s"), CURRENT_CONTEXT, *SpawnerSettingId.ToString());
		return;
	}
	
	SpawnerSettingData = NewSettings;
	ClampSettingDataValues(SpawnerSettingData);
}

void ACharacterSpawnManager::UpdateWaveData()
{
	FWavesData NewWaveData;
	if (!DataTableHelper::GetDataFromDataTable(WaveDataTable, WaveId, NewWaveData))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Update Wave Failed : %s"), CURRENT_CONTEXT, *WaveId.ToString());
		return;
	}
	
	WavesData = NewWaveData;
}

void ACharacterSpawnManager::ClampSettingDataValues(FSpawnerSettingData& Setting)
{
	Setting.SpawnInterval = FMath::Min(Setting.SpawnInterval, 0.2f);
	Setting.SpawnDistanceMin = FMath::Clamp(Setting.SpawnDistanceMin, 0.0f, Setting.SpawnDistanceMax);
	Setting.SpawnDistanceMax = FMath::Max(Setting.SpawnDistanceMax, Setting.SpawnDistanceMin);
}

FWaveData ACharacterSpawnManager::GetWaveData(const int32 WaveIndex)
{
	if (!WavesData.Waves.IsValidIndex(WaveIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Wave index %d is out of bounds. Total waves: %d"), CURRENT_CONTEXT, WaveIndex, WavesData.Waves.Num());
		return FWaveData();
	}
	
	return WavesData.Waves[WaveIndex];
}

FVector ACharacterSpawnManager::GetPlayersCentroid() const
{
	if (ActivePlayers.IsEmpty())
	{
		return FVector::ZeroVector;
	}

	FVector SumLocation = FVector::ZeroVector;
	int32 ValidPlayerCount = 0;

	for (AActor* PlayerActor : ActivePlayers)
	{
		if (IsValid(PlayerActor))
		{
			SumLocation += PlayerActor->GetActorLocation();
			ValidPlayerCount++;
		}
	}

	return (ValidPlayerCount > 0) ? (SumLocation / static_cast<float>(ValidPlayerCount)) : FVector::ZeroVector;
}

FVector ACharacterSpawnManager::FindRandomSpawnLocation(float MinDistance, float MaxDistance)
{
	// Find the Radius
	FSpawnerSettingData* Data = GetCurrentSettingData();
	float Radius = (MaxDistance - MinDistance) / 2.0f;
	
	// find the ring's distance from the player
	float Distance = Radius + MinDistance;

	// find the amount of spheres that can fill the ring
	float Circumference = 2 * PI * Distance;
	float Spacing = MaxDistance - MinDistance;
	float Amount = FMath::CeilToInt(Circumference / Spacing);

	// find the ring with spheres to find valid random spawn positions
	TArray<FVector> PossibleLocation;
	
	FVector CenterLocation = GetPlayersCentroid();

	for (int32 i = 0; i < Amount; ++i)
	{
		TArray<FVector> PossibleHeights;
		FVector OptimalHeight = FVector::ZeroVector;

		float YawAngle = i * (360.0f / Amount);
		FRotator YawRot(0.0f, YawAngle, 0.0f);
		FVector Forward = YawRot.Vector();
		
		FVector Start = CenterLocation + (Distance * Forward);
		FVector End = Start;
		Start.Z += Data->SpawnHeightLimit;
		End.Z -= Data->SpawnHeightLimit;

		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		for (AActor* _ : ActivePlayers)
		{
			Params.AddIgnoredActors(ActivePlayers);
		}
		
		FCollisionObjectQueryParams ObjectTypes;
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);

		bool bHit = GetWorld()->SweepMultiByObjectType(HitResults, Start, End, FQuat::Identity, ObjectTypes, FCollisionShape::MakeSphere(Radius), Params);
		
		if (Data->bDebug)
		{
			float DebugDuration = Data->SpawnInterval;
			FColor SweepColor = bHit ? FColor::Red : FColor::Green;
			DrawDebugSphere(GetWorld(), Start, Radius, 12, SweepColor, false, DebugDuration);
			DrawDebugSphere(GetWorld(), End, Radius, 12, SweepColor, false, DebugDuration);
			DrawDebugLine(GetWorld(), Start, End, SweepColor, false, DebugDuration, 0, 3.0f);
    
			// for (int32 j = 0; j < HitResults.Num(); j++)
			// {
			// 	const FHitResult& Hit = HitResults[j];
			// 	DrawDebugSphere(GetWorld(), Hit.Location, 8.0f, 8, FColor::Orange, false, DebugDuration);
			// 	DrawDebugDirectionalArrow(GetWorld(), Hit.Location, 
			// 		Hit.Location + Hit.Normal * 50.0f, 20.0f, FColor::Blue, false, DebugDuration);
		}

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			UPrimitiveComponent* HitComponent = Hit.GetComponent();
			FName Tag = Data->Tag;

			bool bIsValid = false;

			if (IsValid(HitActor))
			{
				if (Data->bUseTagSystem)
				{
					if (HitActor->ActorHasTag(Tag))
					{
						bIsValid = true;
					}
					else if (IsValid(HitComponent) && HitComponent->ComponentHasTag(Tag))
					{
						bIsValid = true;
					}
				}
				else
				{
					bIsValid = true;
				}
			}

			if (bIsValid)
			{
				FVector HitLocation = Hit.Location;
				HitLocation.Z = Hit.ImpactPoint.Z;
				PossibleHeights.Add(HitLocation);
			}
		}

		if (!PossibleHeights.IsEmpty())
		{
			if (Data->bOptimalHeightSpawn)
			{
				if (PossibleHeights.Num() > 1)
				{
					float OptimalDistance = 0.0f;
					bool Comparing = false;
					for (const FVector& PossibleHeight : PossibleHeights)
					{
						float A = FMath::Abs(PossibleHeight.Z - CenterLocation.Z);
						if (Comparing && A < OptimalDistance)
						{
							OptimalDistance = A;
							OptimalHeight = PossibleHeight;
						}
						else
						{
							Comparing = true;
							OptimalDistance = A;
							OptimalHeight = PossibleHeight;
						}
					}
				}
				else
				{
					OptimalHeight = PossibleHeights[0];
				}
			}
			else
			{
				FRandomStream Stream;
				Stream.GenerateNewSeed();
				CollectionHelper::ShuffleArray(Stream, PossibleHeights);
				OptimalHeight = CollectionHelper::GetRandomElementInArray(PossibleHeights);
			}

			const FVector RandomLocation = GetRandomLocationInRadius(OptimalHeight, Radius, Data->bDebug);
			if (!RandomLocation.Equals(FVector::ZeroVector))
			{
				PossibleLocation.Add(RandomLocation);
			}
		}
	}
	
	if (PossibleLocation.IsEmpty())
	{
		return FVector::ZeroVector;
	}
	
	FRandomStream Stream;
	Stream.GenerateNewSeed();
	CollectionHelper::ShuffleArray(Stream, PossibleLocation);
	
	return CollectionHelper::GetRandomElementInArray(PossibleLocation);
}

FVector ACharacterSpawnManager::GetRandomLocationInRadius(const FVector& OptimalHeight, const float Radius, const bool bDebug) const
{
	if (bDebug)
	{
						
	}
	
	FVector RandomLocation;
	const bool bLocationFound = UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
		GetWorld(),
		OptimalHeight,
		RandomLocation,
		Radius);

	const bool bIsNearlyEqual = OptimalHeight.Equals(RandomLocation, Radius);
	if (bLocationFound && bIsNearlyEqual)
	{
		return RandomLocation;
	}

	// UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to find a random location in radius for OptimalHeight: %s, Radius: %f "), CURRENT_CONTEXT, *OptimalHeight.ToString(), Radius);
	return FVector::ZeroVector;
}

void ACharacterSpawnManager::SpawnEnemies()
{
	if (!HasAuthority()) return;

	const FVector CenterLocation = GetPlayersCentroid();
	if (CenterLocation.IsZero())
	{
		return;
	}

	FWaveData WaveData = GetWaveData(CurrentWaveIndex);
	const FSpawnerSettingData* SettingData = GetCurrentSettingData();
	
	for (const FSingleEnemyData& EnemyData : WaveData.Enemies)
	{
		if (SettingData->MaxEnemies <= (SpawnedEnemies + QueuedEnemies))
			return;

		if (!CheckSpawnChance(EnemyData.SpawnChance))
			continue; 

		const int32 SpawnMin = FMath::Clamp(EnemyData.SpawnMin, 0, EnemyData.SpawnMin);
		const int32 SpawnMax = FMath::Clamp(EnemyData.SpawnMax, EnemyData.SpawnMin, SettingData->MaxEnemies);
		const int32 SpawnAmount = GetRandomSpawnCount(SpawnMin, SpawnMax);

		for (int i = 0; i < SpawnAmount; i++)
		{
			if (SettingData->MaxEnemies <= (SpawnedEnemies + QueuedEnemies))
				return;

			FVector SpawnLocation = FindRandomSpawnLocation(SettingData->SpawnDistanceMin, SettingData->SpawnDistanceMax);
			if (SpawnLocation.Equals(FVector::ZeroVector))
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to find a valid spawn location for enemy type: %s"), CURRENT_CONTEXT, *EnemyData.Class->GetName());
				continue;
			}

			SpawnLocation.Z += EnemyData.CapsuleHalfHeight;
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, CenterLocation);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SpawnLocation);
			SpawnTransform.SetRotation(FQuat(LookAtRotation));
			SpawnTransform.SetScale3D(FVector::OneVector);

			FPendingSpawnInfo Info = { EnemyData.Class, SpawnTransform };
			SpawnQueue.Enqueue(Info);
			QueuedEnemies++; 
		}
	}

	if (!SpawnQueue.IsEmpty() && !GetWorld()->GetTimerManager().IsTimerActive(ProcessQueueTimerHandle))
	{
		GetWorld()->GetTimerManager().SetTimer(
			ProcessQueueTimerHandle, 
			this, 
			&ACharacterSpawnManager::ProcessSpawnQueue, 
			0.05f, 
			true
		);
	}
}

void ACharacterSpawnManager::ProcessSpawnQueue()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(SPAWNMANAGER_ProcessSpawnQueue);
	if (!HasAuthority()) return;
	
	FPendingSpawnInfo SpawnInfo;
	
	if (SpawnQueue.Dequeue(SpawnInfo))
	{
		QueuedEnemies--; 

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		SpawnParams.Instigator = GetInstigator();

		const AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnInfo.EnemyClass, SpawnInfo.SpawnTransform, SpawnParams);
		if (SpawnedActor)
		{
			SpawnedEnemies++;
		}
	}

	if (SpawnQueue.IsEmpty())
	{
		GetWorld()->GetTimerManager().ClearTimer(ProcessQueueTimerHandle);
	}
}

FZombieData* ACharacterSpawnManager::GetZombieData(const EZombieSubType Type)
{
	const FString RowName = EnumHelper::EnumToString(Type);
	if (ZombieDataMap.Contains(RowName))
	{
		return ZombieDataMap[RowName];
	}
	
	UE_LOG(LogTemp, Error, TEXT("[%s] No Zombie data found for type: %d"), CURRENT_CONTEXT, (int32)Type);
	return nullptr;
}
