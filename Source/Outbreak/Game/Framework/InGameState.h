#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "InGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerListUpdate);

UCLASS()
class OUTBREAK_API AInGameState : public AGameStateBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	
public:
	AInGameState();
	
// --------------------
// Getters
// --------------------
	
	FORCEINLINE float GetMatchTime() const { return MatchTime; }
	FORCEINLINE FString GetCurrentPhase() const { return CurrentPhase; }
	
// --------------------
// Variables
// --------------------
	
public:
	// --------Spawner--------
	UPROPERTY()
	AActor* SpawnerInstance;
	
	// --------GameState Info--------
	UPROPERTY(Replicated)
	float MatchTime; // 게임 진행 시간

	UPROPERTY(Replicated)
	FString CurrentPhase; // 현재 페이즈
	
	UPROPERTY(ReplicatedUsing = OnRep_TotalZombieKills)
	int32 TotalZombieKills; // 전체 좀비 킬 수

	UPROPERTY(ReplicatedUsing = OnRep_AlivePlayerCount)
	int32 AlivePlayerCount; // 살아있는 플레이어 수

	// Delegates
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerListUpdate OnPlayerListChanged;
// --------------------
// Functions
// --------------------

public:	
	UFUNCTION()
	void AddTotalZombieKill();
	
protected:
	UFUNCTION()
	void OnRep_TotalZombieKills();

	UFUNCTION()
	void OnRep_AlivePlayerCount();
	
};
