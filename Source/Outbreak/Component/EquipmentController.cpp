// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentController.h"

#include "Net/UnrealNetwork.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Equipment/EquipmentBase.h"
#include "Outbreak/Game/Equipment/Weapon/FirableBase.h"
#include "Outbreak/Game/Equipment/Weapon/WeaponBase.h"
#include "Outbreak/Game/Equipment/Weapon/ThrowableBase.h"
#include "Outbreak/Game/Equipment/Medicine/MedicineBase.h"
#include "Outbreak/Game/Equipment/Weapon/MeleeBase.h"
#include "Outbreak/Game/Framework/DefaultPlayerState.h"
#include "Outbreak/UI/InGameHUD.h"
#include "Outbreak/Util/Define.h"
#include "Outbreak/Util/EnumHelper.h"
#include "Outbreak/UI/WeaponContainer.h"
#include "Utilities/DebugHelper.h"

UEquipmentController::UEquipmentController()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UEquipmentController::BeginPlay()
{
	Super::BeginPlay();
	CachedOwner = Cast<ACharacterPlayer>(GetOwner());
	if (!CachedOwner)
	{
		PRINT_WITH_CURRENT_CONTEXT("Failed to cast ACharacterPlayer");
	}
}

void UEquipmentController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentEquippedType);
	DOREPLIFETIME(ThisClass, CurrentEquippedItem);
	DOREPLIFETIME(ThisClass, FirstPrimaryWeapon);
	DOREPLIFETIME(ThisClass, SecondPrimaryWeapon);
	DOREPLIFETIME(ThisClass, SecondaryWeapon);
	DOREPLIFETIME(ThisClass, ThrowableWeapon);
	DOREPLIFETIME(ThisClass, FirstMedicine);
	DOREPLIFETIME(ThisClass, SecondMedicine);
	DOREPLIFETIME(ThisClass, bIsReload);
}

void UEquipmentController::EquipBySlot(const int32 SlotNumber)
{
	if (IsValid(CurrentEquippedItem) && CurrentEquippedItem->IsActive()) return;
	
	TObjectPtr<AEquipmentBase> TargetItem = nullptr;

	switch (SlotNumber)
	{
	case 1:
		TargetItem = FirstPrimaryWeapon;
		break;
	case 2:
		TargetItem = SecondPrimaryWeapon;
		break;
	case 3:
		TargetItem = SecondaryWeapon;
		break;
	case 4:
		TargetItem = ThrowableWeapon;
		break;
	case 5:
		TargetItem = FirstMedicine;
		break;
	case 6:
		TargetItem = SecondMedicine;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid Slot Number: %d"), CURRENT_CONTEXT, SlotNumber);
		return;
	}

	if (IsValid(TargetItem) && TargetItem != CurrentEquippedItem)
	{
		if (GetOwner() && !GetOwner()->HasAuthority())
		{
			CurrentEquippedType = TargetItem->GetEquipmentType();
			CurrentEquippedItem = TargetItem;
			Equip(TargetItem);
			Server_EquipBySlot(SlotNumber);
			return;
		}
		Equip(TargetItem);
	}
}

void UEquipmentController::AddEquipment(const TObjectPtr<AEquipmentBase>& Equipment)
{
	AInGameHUD* Hud = GetInGameHUD();

	if (!IsValid(Equipment)) return;
	
	EEquipmentType EquipmentType = Equipment->GetEquipmentType();
	switch (EquipmentType)
	{
		case EEquipmentType::PrimaryWeapon:
		{
			AFirableBase* NewPrimaryWeapon = Cast<AFirableBase>(Equipment);
			if (!NewPrimaryWeapon) return;
		
			if (!IsValid(FirstPrimaryWeapon))
			{
				FirstPrimaryWeapon = NewPrimaryWeapon;
				Equip(FirstPrimaryWeapon);
			}
			else if (!IsValid(SecondPrimaryWeapon))
			{
				SecondPrimaryWeapon = NewPrimaryWeapon;
				Equip(SecondPrimaryWeapon);
			}
			else
			{
				if (CurrentEquippedItem == FirstPrimaryWeapon)
				{
					FirstPrimaryWeapon->Destroy();
					FirstPrimaryWeapon = NewPrimaryWeapon;
					Equip(FirstPrimaryWeapon);
				}
				else
				{
					SecondPrimaryWeapon->Destroy();
					SecondPrimaryWeapon = NewPrimaryWeapon;
					Equip(SecondPrimaryWeapon);
				}
			}
			UTexture2D* Icon = Equipment->GetEquipmentIcon();
			if (Icon)
			{
				// Hud->SetWeaponContainer(Icon);
				UE_LOG(LogTemp, Log, TEXT("Icon Load Success"));
			}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Icon Load Fail"));
				}
			break;
		}
		
		case EEquipmentType::SecondaryWeapon:
		{
			AWeaponBase* NewSecondaryWeapon = Cast<AWeaponBase>(Equipment);
			if (!NewSecondaryWeapon) return;
				

				
			if (IsValid(SecondaryWeapon))
			{
				SecondaryWeapon->Destroy();
			}
			SecondaryWeapon = NewSecondaryWeapon;
			Equip(SecondaryWeapon);
			UTexture2D* Icon = Equipment->GetEquipmentIcon();
			if (Icon)
			{
				// Hud->SetSubWeaponContainer(Icon,2 );
				UE_LOG(LogTemp, Log, TEXT("Sub/Icon Load Success"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Sub/Icon Load Fail"));
			}
			break;
		}
		
		case EEquipmentType::ThrowableWeapon:
		{
			AThrowableBase* NewThrowableWeapon = Cast<AThrowableBase>(Equipment);
			if (!NewThrowableWeapon) return;
		
			if (IsValid(ThrowableWeapon))
			{
				ThrowableWeapon->Destroy();
			}
			ThrowableWeapon = NewThrowableWeapon;
			Equip(ThrowableWeapon);
				UTexture2D* Icon = Equipment->GetEquipmentIcon();
				if (Icon)
				{
					// Hud->SetBottomInv(Icon,1);
					UE_LOG(LogTemp, Log, TEXT("Throw/Icon Load Success"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Throw/Icon Load Fail"));
				}
			break;
		}
		
		case EEquipmentType::Medicine:
		{
			AMedicineBase* NewMedicine = Cast<AMedicineBase>(Equipment);
			if (!NewMedicine) return;

			if (!IsValid(FirstMedicine))
			{
				FirstMedicine = NewMedicine;
				Equip(FirstMedicine);
				UTexture2D* Icon = Equipment->GetEquipmentIcon();
				if (Icon)
				{
					// Hud->SetBottomInv(Icon,2);
					UE_LOG(LogTemp, Log, TEXT("Med1/Icon Load Success"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Med1/Icon Load Fail"));
				}
			}
			else if (!IsValid(SecondMedicine))
			{
				SecondMedicine = NewMedicine;
				Equip(SecondMedicine);
				UTexture2D* Icon = Equipment->GetEquipmentIcon();
				if (Icon)
				{
					// Hud->SetBottomInv(Icon,3);
					UE_LOG(LogTemp, Log, TEXT("Med2/Icon Load Success"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Med2/Icon Load Fail"));
				}
			}
			else
			{
				FirstMedicine->Destroy();
				FirstMedicine = NewMedicine;
				Equip(FirstMedicine);
				UTexture2D* Icon = Equipment->GetEquipmentIcon();
				if (Icon)
				{
					// Hud->SetBottomInv(Icon,2);
					UE_LOG(LogTemp, Log, TEXT("Med1/Icon Load Success"));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Med1/Icon Load Fail"));
				}
			}
			break;
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid Equipment Type: %s"), CURRENT_CONTEXT, *EnumHelper::EnumToString(EquipmentType));
			return;
	}

}

void UEquipmentController::PickupEquipment(class AEquipmentBase* NewItem)
{
	if (!IsValid(NewItem)) return;
	if (!GetOwner()->HasAuthority()) return;

	if (NewItem->GetOwner() != nullptr && NewItem->GetOwner() != CachedOwner) return;
	AEquipmentBase* CurrentItem = nullptr;
	EEquipmentType Type = NewItem->GetEquipmentType();

	if (NewItem->bIsAmmoPack)
	{
		if (ADefaultPlayerState* PS = CachedOwner->GetPlayerState<ADefaultPlayerState>())
		{
			PS->AddAmmo(NewItem->SupplyAmmoType, NewItem->SupplyAmount);
			NewItem->Destroy();
			return;
		}
	}
	switch (Type)
	{
		case EEquipmentType::PrimaryWeapon:
			if (IsValid(FirstPrimaryWeapon) && FirstPrimaryWeapon->GetClass() == NewItem->GetClass()) CurrentItem = FirstPrimaryWeapon;
			else if (IsValid(SecondPrimaryWeapon) && SecondPrimaryWeapon->GetClass() == NewItem->GetClass()) CurrentItem = SecondPrimaryWeapon;
			else if (IsValid(FirstPrimaryWeapon)) CurrentItem = FirstPrimaryWeapon;
			break;
	case EEquipmentType::SecondaryWeapon:
		if (IsValid(SecondaryWeapon) && SecondaryWeapon->GetClass() == NewItem->GetClass())
			CurrentItem = SecondaryWeapon;
		break;
	case EEquipmentType::ThrowableWeapon:
		if (IsValid(ThrowableWeapon) && ThrowableWeapon->GetClass() == NewItem->GetClass())
			CurrentItem = ThrowableWeapon;
		break;
	case EEquipmentType::Medicine:
		if (IsValid(FirstMedicine) && FirstMedicine->GetClass() == NewItem->GetClass())
			CurrentItem = FirstMedicine;
		else if (IsValid(SecondMedicine) && SecondMedicine->GetClass() == NewItem->GetClass())
			CurrentItem = SecondMedicine;
		break;
	}

	ADefaultPlayerState* PS = CachedOwner->GetPlayerState<ADefaultPlayerState>();
	
	if (IsValid(CurrentItem))
	{
		if (PS)
		{
			if (AFirableBase* FirableItem = Cast<AFirableBase>(NewItem))
			{
				PS->AddAmmo(FirableItem->GetFirableData().FirableType, FirableItem->GetFirableData().MagazineSize);
			}
			else if (AThrowableBase* ThrowableNew = Cast<AThrowableBase>(NewItem))
			{
				PS->AddThrowable(ThrowableNew->GetThrowableData().ThrowableType, ThrowableNew->GetThrowableData().MaxCount);
			}
			else if (AMedicineBase* MedicineNew = Cast<AMedicineBase>(NewItem))
			{
				// [변경] 약품 개수 추가
				PS->AddMedicine(MedicineNew->GetMedicineData().MedicineType, MedicineNew->GetMedicineData().MaxCount);
			}
		}
		if (!IsValid(CurrentEquippedItem)) Equip(CurrentItem);
		NewItem->Destroy();
		return;
	}
	AEquipmentBase* ItemToDrop = nullptr;

	switch (Type)
	{
		case EEquipmentType::PrimaryWeapon:
			if (!IsValid(FirstPrimaryWeapon) || !IsValid(SecondPrimaryWeapon))
			{
				ItemToDrop = nullptr; 
			}
			else 
			{
				if (CurrentEquippedItem == FirstPrimaryWeapon) ItemToDrop = FirstPrimaryWeapon;
				else if (CurrentEquippedItem == SecondPrimaryWeapon) ItemToDrop = SecondPrimaryWeapon;
				else ItemToDrop = FirstPrimaryWeapon;
			}
			break;
		case EEquipmentType::SecondaryWeapon:
				if (IsValid(SecondaryWeapon)) ItemToDrop = SecondaryWeapon;
			break;
		case EEquipmentType::ThrowableWeapon:
			if (IsValid(ThrowableWeapon)) ItemToDrop = ThrowableWeapon;
			break;
		case EEquipmentType::Medicine:
				if (!IsValid(FirstMedicine) || !IsValid(SecondMedicine))
				{
					ItemToDrop = nullptr;
				}
				else
				{
					if (CurrentEquippedItem == FirstMedicine) ItemToDrop = FirstMedicine;
					else if (CurrentEquippedItem == SecondMedicine) ItemToDrop = SecondMedicine;
					else ItemToDrop = FirstMedicine;
				}
			break;
		}
		if (IsValid(ItemToDrop))
		{
			DropEquipment(ItemToDrop);
			RemoveEquipment(ItemToDrop);
		}
	if (PS)
	{
		if (AThrowableBase* ThrowableNew = Cast<AThrowableBase>(NewItem))
		{
			PS->AddThrowable(ThrowableNew->GetThrowableData().ThrowableType, ThrowableNew->GetThrowableData().MaxCount);
		}
		else if (AMedicineBase* MedicineNew = Cast<AMedicineBase>(NewItem))
		{
			PS->AddMedicine(MedicineNew->GetMedicineData().MedicineType, MedicineNew->GetMedicineData().MaxCount);
		}
	}
	NewItem->SetOwner(CachedOwner); 
	NewItem->SetInstigator(CachedOwner);
	NewItem->SetPhysicsStatus(false);
	AddEquipment(NewItem);
	NewItem->ForceNetUpdate();
}

void UEquipmentController::DropEquipment(class AEquipmentBase* ItemToDrop)
{
	if (!IsValid(ItemToDrop)) return;
	ItemToDrop->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	
	FVector DropLocation = CachedOwner->GetActorLocation() + (CachedOwner->GetActorForwardVector() * 100.0f);
	ItemToDrop->SetActorLocation(DropLocation);
	ItemToDrop->SetPhysicsStatus(true);
	ItemToDrop->SetOwner(nullptr);
}


void UEquipmentController::HandleUse()
{
	if (!IsValid(CurrentEquippedItem))
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		Server_HandleUse();
		return;
	}

	if (!CurrentEquippedItem->CanUse())
		return;
	
	CurrentEquippedItem->OnUse();
	bIsOnUse = true;
}

void UEquipmentController::HandleEndUse()
{
	if (!IsValid(CurrentEquippedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Not Valid Current Equipped Item"), CURRENT_CONTEXT);
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		Server_HandleEndUse();
		return;
	}
	CurrentEquippedItem->OnEndUse();
	bIsOnUse = false;
}

void UEquipmentController::HandleReload()
{
	if (!IsValid(CurrentEquippedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Not Valid Current Equipped Item"), CURRENT_CONTEXT);
		return;
	}
	
	AFirableBase* CurrentFirable = Cast<AFirableBase>(CurrentEquippedItem);
	if (!CurrentFirable)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Current Equipped Item is not a FirableBase"), CURRENT_CONTEXT);
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		Server_HandleReload();
		return;
	}

	if (!CurrentFirable->CanReload())
		return;

	FOnReloadFinished DoneCallback;
	DoneCallback.BindUObject(this, &UEquipmentController::OnReloadFinished);

	CurrentFirable->StartReload(DoneCallback);
	bIsReload = true;
}

void UEquipmentController::HandleToggleFireMode()
{
	if (!IsValid(CurrentEquippedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Not Valid Current Equipped Item"), CURRENT_CONTEXT);
		return;
	}

	AFirableBase* CurrentFirable = Cast<AFirableBase>(CurrentEquippedItem);
	if (CurrentFirable)
	{
		CurrentFireType = CurrentFirable->ToggleFireMode();
	}
}

void UEquipmentController::Equip(const TObjectPtr<AEquipmentBase>& Equipment)
{
    if (GetOwner() && GetOwner()->HasAuthority())
    {
        EquipInternal(Equipment);
    }
}

void UEquipmentController::EquipInternal(AEquipmentBase* Equipment)
{
    if (!IsValid(Equipment) || !IsValid(CachedOwner))
    {
        return;
    }

    UnEquipCurrentEquipment();
    CurrentEquippedItem = Equipment;

    if (GetOwner() && GetOwner()->HasAuthority())
    {
        CurrentEquippedType = CurrentEquippedItem->GetEquipmentType();
    }

    FName SocketName = TEXT("Weapon_M4");

    switch (CurrentEquippedItem->GetEquipmentType())
    {
        case EEquipmentType::SecondaryWeapon:
            SocketName = TEXT("Weapon_Knife");
            break;
        case EEquipmentType::ThrowableWeapon:
            SocketName = TEXT("Weapon_Granade");
            break;
        case EEquipmentType::Medicine:
            SocketName = TEXT("Weapon_FirstAidKit");
            break;
        case EEquipmentType::PrimaryWeapon:
        default:
            SocketName = TEXT("Weapon_M4");
            break;
    }

    CurrentEquippedItem->AttachToComponent(
        CachedOwner->GetMesh(), 
        FAttachmentTransformRules::SnapToTargetNotIncludingScale, 
        SocketName
    );
    
    CurrentEquippedItem->SetActorHiddenInGame(false);
    if (USkeletalMeshComponent* EquipMesh = CurrentEquippedItem->FindComponentByClass<USkeletalMeshComponent>())
    {
        EquipMesh->SetVisibility(true);
        EquipMesh->SetHiddenInGame(false);
    }

    if (AFirableBase* NewFirableWeapon = Cast<AFirableBase>(CurrentEquippedItem))
    {
        CurrentFireType = NewFirableWeapon->GetCurrentFireType();
        CurrentAmmoInMag = NewFirableWeapon->GetCurrentAmmoInMag();
        NewFirableWeapon->OnPlayerAmmoChangedDelegate.AddDynamic(this, &UEquipmentController::HandleAmmoChanged);

        if (ADefaultPlayerState* PS = CachedOwner->GetPlayerState<ADefaultPlayerState>())
        {
            PS->OnPlayerAmmoChangedDelegate.AddDynamic(this, &UEquipmentController::HandleAmmoChanged);
        }
    }
    
    CurrentEquippedItem->OnEquip();
    HandleAmmoChanged();
    
    UE_LOG(LogTemp, Log, TEXT("[%s] Equipped: %s (Authority: %s)"), 
           CURRENT_CONTEXT, 
           *CurrentEquippedItem->GetName(),
           GetOwner()->HasAuthority() ? TEXT("YES") : TEXT("NO"));
}

void UEquipmentController::UnEquipCurrentEquipment()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->SetActorHiddenInGame(true);
		
		if (AFirableBase* OldFirableWeapon = Cast<AFirableBase>(CurrentEquippedItem))
		{
			OldFirableWeapon->OnPlayerAmmoChangedDelegate.RemoveDynamic(this, &UEquipmentController::HandleAmmoChanged);
			if (IsValid(CachedOwner))
			{
				if (ADefaultPlayerState* PS = CachedOwner->GetPlayerState<ADefaultPlayerState>())
				{
					PS->OnPlayerAmmoChangedDelegate.RemoveDynamic(this, &UEquipmentController::HandleAmmoChanged);
				}
			}
		}
		if (AMeleeBase* OldMeleeWeapon = Cast<AMeleeBase>(CurrentEquippedItem))
		{
			OldMeleeWeapon->ResetAttack();
		}
	}
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		CurrentEquippedType = EEquipmentType::None;
	}
	CurrentEquippedItem = nullptr;
}

void UEquipmentController::RemoveEquipment(AEquipmentBase* ItemToRemove)
{
	if (!IsValid(ItemToRemove)) return;

	if (CurrentEquippedItem == ItemToRemove)
	{
		UnEquipCurrentEquipment();
	}

	if (FirstPrimaryWeapon == ItemToRemove)
	{
		FirstPrimaryWeapon = nullptr;
	}
	else if (SecondPrimaryWeapon == ItemToRemove)
	{
		SecondPrimaryWeapon = nullptr;
	}
	else if (SecondaryWeapon == ItemToRemove)
	{
		SecondaryWeapon = nullptr;
	}
	else if (ThrowableWeapon == ItemToRemove)
	{
		ThrowableWeapon = nullptr;
	}
	else if (FirstMedicine == ItemToRemove)
	{
		FirstMedicine = nullptr;
	}
	else if (SecondMedicine == ItemToRemove)
	{
		SecondMedicine = nullptr;
	}

	ItemToRemove->Destroy();
}


void UEquipmentController::HandleAmmoChanged()
{
	const AFirableBase* CurrentWeapon = Cast<AFirableBase>(CurrentEquippedItem);
	if (!CurrentWeapon) return;

	const ADefaultPlayerState* PS = CachedOwner->GetPlayerState<ADefaultPlayerState>();
	if (!PS) return;

	const int32 CurrentMag = CurrentWeapon->GetCurrentAmmoInMag();
	const int32 ReserveAmmo = PS->GetReserveAmmo(CurrentWeapon->GetFirableData().FirableType);

	if (AInGameHUD* Hud = GetInGameHUD())
	{
		Hud->DisplayAmmo(CurrentMag, ReserveAmmo);
	}
}

void UEquipmentController::OnRep_CurrentEquippedItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
		{
			if (IsValid(CurrentEquippedItem))
			{
				EquipInternal(CurrentEquippedItem);
			}
		}, 0.1f, false);
	}
	else
	{
		UnEquipCurrentEquipment();
		CurrentEquippedType = EEquipmentType::None;
	}
}

void UEquipmentController::OnRep_CurrentEquippedType()
{
	UE_LOG(LogTemp, Warning, TEXT("CurrentEquippedType Replicated: %d"), (int32)CurrentEquippedType);
}

void UEquipmentController::Server_EquipBySlot_Implementation(int32 SlotNumber)
{
	EquipBySlot(SlotNumber);
}
void UEquipmentController::Server_HandleUse_Implementation()
{
	HandleUse();
}
void UEquipmentController::Server_HandleEndUse_Implementation()
{
	HandleEndUse();
}
void UEquipmentController::Server_HandleReload_Implementation()
{
	HandleReload();
}

void UEquipmentController::OnRep_FirstPrimaryWeapon()
{
	if (CurrentEquippedItem == FirstPrimaryWeapon && IsValid(FirstPrimaryWeapon))
	{
		EquipInternal(FirstPrimaryWeapon);
	}
}

void UEquipmentController::OnRep_SecondPrimaryWeapon()
{
	if (CurrentEquippedItem == SecondPrimaryWeapon && IsValid(SecondPrimaryWeapon))
	{
		EquipInternal(SecondPrimaryWeapon);
	}
}

void UEquipmentController::OnRep_SecondaryWeapon()
{
	if (CurrentEquippedItem == SecondaryWeapon && IsValid(SecondaryWeapon))
	{
		EquipInternal(SecondaryWeapon);
	}
}

void UEquipmentController::OnRep_ThrowableWeapon()
{
	if (CurrentEquippedItem == ThrowableWeapon && IsValid(ThrowableWeapon))
	{
		EquipInternal(ThrowableWeapon);
	}
}

void UEquipmentController::OnRep_FirstMedicine()
{
	if (CurrentEquippedItem == FirstMedicine && IsValid(FirstMedicine))
	{
		EquipInternal(FirstMedicine);
	}
}

void UEquipmentController::OnRep_SecondMedicine()
{
	if (CurrentEquippedItem == SecondMedicine && IsValid(SecondMedicine))
	{
		EquipInternal(SecondMedicine);
	}
}

AInGameHUD* UEquipmentController::GetInGameHUD()
{
	if (CachedHUD.IsValid()) return CachedHUD.Get();

	const ACharacterPlayer* Character = GetCachedOwner();
	if (!Character) return nullptr;

	const APlayerController* PC = Cast<APlayerController>(Character->GetController());
    
	if (PC && PC->IsLocalController())
	{
		CachedHUD = Cast<AInGameHUD>(PC->GetHUD());
		if (CachedHUD.IsValid())
		{
			return CachedHUD.Get();
		}
	}

	return nullptr;
}
