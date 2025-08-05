// Fill out your copyright notice in the Description page of Project Settings.
#include "CharacterSpawnManager.h"
#include "CharacterFactory.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Data/GameData.h"
#include "Outbreak/Util/CollectionHelper.h"
#include "Outbreak/Util/EnumHelper.h"

ACharacterSpawnManager::ACharacterSpawnManager()
{
	CharacterFactory = CreateDefaultSubobject<UCharacterFactory>(TEXT("CharacterFactory"));
	static ConstructorHelpers::FObjectFinder<UDataTable> ZombieDataTableAsset(TEXT("/Script/Engine.DataTable'/Game/Data/ZombieDataTable.ZombieDataTable'"));
	if (ZombieDataTableAsset.Succeeded())
	{
		ZombieDataTable = ZombieDataTableAsset.Object;
	}
	static ConstructorHelpers::FObjectFinder<UDataTable> PlayerDataTableAsset(TEXT("/Script/Engine.DataTable'/Game/Data/PlayerDataTable.PlayerDataTable'"));
	if (PlayerDataTableAsset.Succeeded())
	{
		PlayerDataTable = PlayerDataTableAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> SpawnSettingDataTableAsset(TEXT("/Script/Engine.DataTable'/Game/Data/DT_SpawnerSetting.DT_SpawnerSetting'"));
	if (SpawnSettingDataTableAsset.Succeeded())
	{
		SpawnerSettingDataTable = SpawnSettingDataTableAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> WaveDataTableAsset(TEXT("/Script/Engine.DataTable'/Game/Data/DT_Waves.DT_Waves'"));
	if (WaveDataTableAsset.Succeeded())
	{
		WaveDataTable = WaveDataTableAsset.Object;
	}
}

void ACharacterSpawnManager::BeginPlay()
{
	Super::BeginPlay();

	LoadDataTableToMap(ZombieDataTable, ZombieDataMap);
	LoadDataTableToMap(PlayerDataTable, PlayerDataMap);

	UpdateSettingData();
	UpdateWaveData();
	ClampSettingDataValues(SpawnerSettingData);
}

void ACharacterSpawnManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
bool ACharacterSpawnManager::GetSettingDataFromDataTable(const FName InSettingsID, FSpawnerSettingData& OutSetting)
{
	if (!SpawnerSettingDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DataTable is null"), CURRENT_CONTEXT);
		return false;
	}

	const FSpawnerSettingData* FoundRow = SpawnerSettingDataTable->FindRow<FSpawnerSettingData>(InSettingsID,TEXT("GetSettingsDataFromDataTable"));

	if (FoundRow)
	{
		OutSetting = *FoundRow;
		return true;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("[%s] Row '%s' not found in data table"), CURRENT_CONTEXT, *InSettingsID.ToString());
	return false;
}

bool ACharacterSpawnManager::GetWaveDataFromDataTable(FName InWaveId, FWavesData& OutWaveData)
{
	if (!WaveDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] DataTable is null"), CURRENT_CONTEXT);
		return false;
	}

	const FWavesData* FoundRow = WaveDataTable->FindRow<FWavesData>(InWaveId, TEXT("GetWaveDataFromDataTable"));

	if (FoundRow)
	{
		OutWaveData = *FoundRow;
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[%s] Row '%s' not found in data table"), CURRENT_CONTEXT, *InWaveId.ToString());
	return false;
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

void ACharacterSpawnManager::Activate(const TObjectPtr<ACharacterPlayer>& InTarget)
{
	if (bIsActivated)
		return;
	
	Target = InTarget;
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
	if (!bIsActivated)
		return;
	
	bIsActivated = false;
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
}

void ACharacterSpawnManager::UpdateSettingData()
{
	FSpawnerSettingData NewSettings;
	if (GetSettingDataFromDataTable(SpawnerSettingId, NewSettings))
	{
		SpawnerSettingData = NewSettings;
		ClampSettingDataValues(SpawnerSettingData);
		UE_LOG(LogTemp, Log, TEXT("[%s] Update Setting Done. : %s"), CURRENT_CONTEXT, *SpawnerSettingId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Update Setting Failed : %s"), CURRENT_CONTEXT, *SpawnerSettingId.ToString());
	}
}

void ACharacterSpawnManager::UpdateWaveData()
{
	FWavesData NewWaveData;
	if (GetWaveDataFromDataTable(WaveId, NewWaveData))
	{
		WavesData = NewWaveData;
		UE_LOG(LogTemp, Log, TEXT("[%s] Update Wave Done. : %s"), CURRENT_CONTEXT, *WaveId.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Update Wave Failed : %s"), CURRENT_CONTEXT, *WaveId.ToString());
	}
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
	
	FWaveData CurrentWaveData = WavesData.Waves[WaveIndex];
	return CurrentWaveData;
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

	for (int32 i = 0; i < Amount; ++i)
	{
		TArray<FVector> PossibleHeights;
		bool Comparing = false;
		float OptimalDistance = 0.0f;
		FVector OptimalHeight = FVector::ZeroVector;

		float YawAngle = i * (360.0f / Amount);
		FRotator YawRot(0.0f, YawAngle, 0.0f);
		FVector Forward = YawRot.Vector();
		
		FVector Start = Target->GetActorLocation() + (Distance * Forward);
		FVector End = Start;
		Start.Z += Data->SpawnHeightLimit;
		End.Z -= Data->SpawnHeightLimit;

		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		Params.AddIgnoredActor(Target); 
		
		FCollisionObjectQueryParams ObjectTypes;
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectTypes.AddObjectTypesToQuery(ECC_WorldDynamic);

		bool bHit = GetWorld()->SweepMultiByObjectType(
			HitResults,
			Start,
			End,
			FQuat::Identity,
			ObjectTypes,
			FCollisionShape::MakeSphere(Radius),
			Params);
		
		// Debugging
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
   //      
			// 	if (Hit.GetActor())
			// 	{
			// 		DrawDebugString(GetWorld(), Hit.Location + FVector(0, 0, 30), 
			// 			FString::Printf(TEXT("%s [%d]"), *Hit.GetActor()->GetName(), j), 
			// 			nullptr, FColor::White, DebugDuration);
			// 	}
			// }
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
					for (const FVector& PossibleHeight : PossibleHeights)
					{
						float A = FMath::Abs(PossibleHeight.Z - Target->GetActorLocation().Z);
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
	if (!Target)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Target not found!"), CURRENT_CONTEXT);
		return;
	}

	FWaveData WaveData = GetWaveData(CurrentWaveIndex);
	const FSpawnerSettingData* SettingData = GetCurrentSettingData();
	
	for (const FSingleEnemyData& EnemyData : WaveData.Enemies)
	{
		if (SettingData->MaxEnemies <= SpawnedEnemies)
		{
			UE_LOG(LogTemp, Log, TEXT("[%s] Exceeded max enemy limit: %d"), CURRENT_CONTEXT, SettingData->MaxEnemies);
			return;
		}

		if (!CheckSpawnChance(EnemyData.SpawnChance))
			return;

		const int32 SpawnMin = FMath::Clamp(EnemyData.SpawnMin, 0, EnemyData.SpawnMin);
		const int32 SpawnMax = FMath::Clamp(EnemyData.SpawnMax, EnemyData.SpawnMin, SettingData->MaxEnemies);
		const int32 SpawnAmount = GetRandomSpawnCount(SpawnMin, SpawnMax);

		for (int i = 0; i < SpawnAmount - 1; i++)
		{
			if (SettingData->MaxEnemies <= SpawnedEnemies)
			{
				UE_LOG(LogTemp, Log, TEXT("[%s] Exceeded max enemy limit: %d"), CURRENT_CONTEXT, SettingData->MaxEnemies);
				return;
			}

			FVector SpawnLocation = FindRandomSpawnLocation(SettingData->SpawnDistanceMin, SettingData->SpawnDistanceMax);
			if (SpawnLocation.Equals(FVector::ZeroVector))
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] Failed to find a valid spawn location for enemy type: %s"), CURRENT_CONTEXT, *EnemyData.Class->GetName());
				continue;
			}

			SpawnLocation.Z += EnemyData.CapsuleHalfHeight;
			FVector TargetLocation = Target->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(SpawnLocation);
			SpawnTransform.SetRotation(FQuat(LookAtRotation));
			SpawnTransform.SetScale3D(FVector::OneVector);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			SpawnParams.Instigator = GetInstigator();

			const bool bIsSpawned = CharacterFactory->SpawnCharacter(EnemyData.Class, SpawnTransform, SpawnParams);
			
			if (bIsSpawned)
			{
				SpawnedEnemies++;
			}
		}
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

FPlayerData* ACharacterSpawnManager::GetPlayerData(const EPlayerType Type)
{
	const FString RowName = EnumHelper::EnumToString(Type);
	if (PlayerDataMap.Contains(RowName))
	{
		return PlayerDataMap[RowName];
	}

	UE_LOG(LogTemp, Error, TEXT("[%s] No Player data found for type: %d"), CURRENT_CONTEXT, (int32)Type);
	return nullptr;
}