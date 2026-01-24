// Fill out your copyright notice in the Description page of Project Settings.

#include "DefaultPlayerState.h"

#include "Framework/GameState/LobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "Outbreak/UI/InGameHUD.h"
#include "Utilities/DebugHelper.h"

ADefaultPlayerState::ADefaultPlayerState()
{
	bReplicates = true;
	ZombieKills = 0;
}

void ADefaultPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
	
	if (ALobbyGameState* Lgs = GetWorld()->GetGameState<ALobbyGameState>())
	{
		Lgs->UpdatePlayerList();
	}
}

void ADefaultPlayerState::AddZombieKill()
{
	ZombieKills++;
	OnRep_ZombieKills();
}

int32 ADefaultPlayerState::GetReserveAmmo(EFirableType Type) const
{
	const FAmmoCount* FoundAmmo = ReserveAmmoArray.FindByPredicate([Type](const FAmmoCount& Item){
		return Item.Type == Type;
	});

	return FoundAmmo ? FoundAmmo->Count : 0;
}

void ADefaultPlayerState::ConsumeAmmo(EFirableType Type, int32 AmountToConsume)
{
	if (GetLocalRole() != ROLE_Authority) return;

	FAmmoCount* FoundAmmo = ReserveAmmoArray.FindByPredicate([Type](const FAmmoCount& Item){
		return Item.Type == Type;
	});

	if (FoundAmmo)
	{
		FoundAmmo->Count = FMath::Max(0, FoundAmmo->Count - AmountToConsume);
		OnPlayerAmmoChangedDelegate.Broadcast();
		ForceNetUpdate(); 
		OnRep_ReserveAmmoArray();
	}
}

void ADefaultPlayerState::AddAmmo(EFirableType Type, const int32 InInAmountToAdd)
{
	if (GetLocalRole() != ROLE_Authority) return;

	FAmmoCount* FoundAmmo = ReserveAmmoArray.FindByPredicate([Type](const FAmmoCount& Item){
	   return Item.Type == Type;
	});

	if (FoundAmmo && AmmoDataMap.Contains(Type))
	{
		const int32 MaxAmmo = AmmoDataMap[Type].MaxTotalAmmo;
		
		if (FoundAmmo->Count >= MaxAmmo) return;

		const int32 OldCount = FoundAmmo->Count;
		const int32 AmountToAdd = InInAmountToAdd == 0 ? AmmoDataMap[Type].ResupplyAmount : InInAmountToAdd;
		FoundAmmo->Count = FMath::Min(FoundAmmo->Count + AmountToAdd, MaxAmmo);

		if (FoundAmmo->Count != OldCount)
		{
			OnPlayerAmmoChangedDelegate.Broadcast();
			ForceNetUpdate();
			OnRep_ReserveAmmoArray();
		}
	}
}

int32 ADefaultPlayerState::GetThrowableCount(EThrowableType Type) const
{
	const FThrowableCount* FoundItem = ThrowableInventory.FindByPredicate([Type](const FThrowableCount& Item){
		return Item.Type == Type;
	});
	return FoundItem ? FoundItem->Count : 0;
}

void ADefaultPlayerState::AddThrowable(EThrowableType Type, int32 Amount)
{
	if (GetLocalRole() != ROLE_Authority) return;

	FThrowableCount* FoundItem = ThrowableInventory.FindByPredicate([Type](const FThrowableCount& Item){
		return Item.Type == Type;
	});

	if (FoundItem)
	{
		FoundItem->Count += Amount;
	}
	else
	{
		FThrowableCount NewItem;
		NewItem.Type = Type;
		NewItem.Count = Amount;
		ThrowableInventory.Add(NewItem);
	}
    OnInventoryChangedDelegate.Broadcast();
	ForceNetUpdate();
}

bool ADefaultPlayerState::ConsumeThrowable(EThrowableType Type, int32 Amount)
{
	if (GetLocalRole() != ROLE_Authority) return false;

	FThrowableCount* FoundItem = ThrowableInventory.FindByPredicate([Type](const FThrowableCount& Item){
		return Item.Type == Type;
	});

	if (FoundItem && FoundItem->Count >= Amount)
	{
		FoundItem->Count -= Amount;
		OnInventoryChangedDelegate.Broadcast();
		ForceNetUpdate();
		return true;
	}
	return false;
}
int32 ADefaultPlayerState::GetMedicineCount(EMedicineType Type) const
{
	const FMedicineCount* FoundItem = MedicineInventory.FindByPredicate([Type](const FMedicineCount& Item){
		return Item.Type == Type;
	});
	return FoundItem ? FoundItem->Count : 0;
}

void ADefaultPlayerState::AddMedicine(EMedicineType Type, int32 Amount)
{
	if (GetLocalRole() != ROLE_Authority) return;

	FMedicineCount* FoundItem = MedicineInventory.FindByPredicate([Type](const FMedicineCount& Item){
		return Item.Type == Type;
	});

	if (FoundItem)
	{
		FoundItem->Count += Amount;
	}
	else
	{
		FMedicineCount NewItem;
		NewItem.Type = Type;
		NewItem.Count = Amount;
		MedicineInventory.Add(NewItem);
	}

	OnInventoryChangedDelegate.Broadcast();
	ForceNetUpdate();
}

bool ADefaultPlayerState::ConsumeMedicine(EMedicineType Type, int32 Amount)
{
	if (GetLocalRole() != ROLE_Authority) return false;

	FMedicineCount* FoundItem = MedicineInventory.FindByPredicate([Type](const FMedicineCount& Item){
		return Item.Type == Type;
	});

	if (FoundItem && FoundItem->Count >= Amount)
	{
		FoundItem->Count -= Amount;
		OnInventoryChangedDelegate.Broadcast();
		ForceNetUpdate();
		return true;
	}
	return false;
}



void ADefaultPlayerState::SetIsDead(bool bDead)
{
	if (HasAuthority())
	{
		bIsDead = bDead;
		ForceNetUpdate();
		OnRep_IsDead();
	}
}

void ADefaultPlayerState::OnRep_IsDead()
{
	// TODO : Show Dead Icons on HUD 
}

void ADefaultPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		InitializeAmmo();
	}
}

void ADefaultPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, ZombieKills);
	DOREPLIFETIME_CONDITION(ThisClass, ReserveAmmoArray, COND_OwnerOnly);
	DOREPLIFETIME(ThisClass, bIsDead);
	DOREPLIFETIME_CONDITION(ThisClass, ThrowableInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ThisClass, MedicineInventory, COND_OwnerOnly);
}

void ADefaultPlayerState::OnRep_Inventory()
{
	OnInventoryChangedDelegate.Broadcast();
}


void ADefaultPlayerState::OnRep_ZombieKills()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalController() && PC->PlayerState == this)
		{
			if (AInGameHUD* HUD = Cast<AInGameHUD>(PC->GetHUD()))
			{
				HUD->DisplayZombieKills(ZombieKills);
			}
		}
	}
}

void ADefaultPlayerState::OnRep_ReserveAmmoArray()
{
	OnPlayerAmmoChangedDelegate.Broadcast();
}

void ADefaultPlayerState::InitializeAmmo()
{
	ReserveAmmoArray.Empty();

	if (!AmmoDataTable)
	{
		PRINT_WITH_CURRENT_CONTEXT("AmmoData is NULL in PlayerState!");
		return;
	}

	const TMap<FName, uint8*>& RowMap = AmmoDataTable->GetRowMap();

	for (const auto& Pair : RowMap)
	{
		const FAmmoData* Data = reinterpret_cast<FAmmoData*>(Pair.Value);

		if (Data)
		{
			FAmmoCount NewAmmo;
			NewAmmo.Type = Data->Type;
			NewAmmo.Count = Data->InitialAmmoCount;
            
			ReserveAmmoArray.Add(NewAmmo);
			AmmoDataMap.Add(Data->Type, *Data);
		}
	}

	OnPlayerAmmoChangedDelegate.Broadcast(); 
    
	ForceNetUpdate();
}
