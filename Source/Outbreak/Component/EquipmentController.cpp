// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentController.h"

#include "Kismet/GameplayStatics.h"
#include "Outbreak/Character/Player/CharacterPlayer.h"
#include "Outbreak/Game/Controller/OBPlayerController.h"
#include "Outbreak/Game/Equipment/EquipmentBase.h"
#include "Outbreak/Game/Equipment/Weapon/FirableBase.h"
#include "Outbreak/Game/Equipment/Weapon/WeaponBase.h"
#include "Outbreak/Game/Equipment/Weapon/ThrowableBase.h"
#include "Outbreak/Game/Equipment/Medicine/MedicineBase.h"
#include "Outbreak/Util/Define.h"
#include "Outbreak/Util/EnumHelper.h"

UEquipmentController::UEquipmentController()
{
	PrimaryComponentTick.bCanEverTick = true;


}

void UEquipmentController::BeginPlay()
{
	Super::BeginPlay();

}


void UEquipmentController::TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEquipmentController::EquipBySlot(const int32 SlotNumber)
{
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
		Equip(TargetItem);
	}
}

void UEquipmentController::AddEquipment(const TObjectPtr<AEquipmentBase>& Equipment)
{
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
			}
			else if (!IsValid(SecondMedicine))
			{
				SecondMedicine = NewMedicine;
				Equip(SecondMedicine);
			}
			else
			{
				FirstMedicine->Destroy();
				FirstMedicine = NewMedicine;
				Equip(FirstMedicine);
			}
			break;
		}
		default:
			UE_LOG(LogTemp, Warning, TEXT("[%s] Invalid Equipment Type: %s"), CURRENT_CONTEXT, *EnumHelper::EnumToString(EquipmentType));
			return;
	}
}

void UEquipmentController::HandleUse()
{
	if (!IsValid(CurrentEquippedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Not Valid Current Equipped Item"), CURRENT_CONTEXT);
		return;
	}
	
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
	CurrentEquippedItem->OnEndUse();
	bIsOnUse = false;
}

void UEquipmentController::HandleReload()
{
	if (!IsValid(CurrentEquippedItem) && bIsReload)
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
	
	CurrentFirable->Reload();
	bIsReload = true;
}

void UEquipmentController::HandleToggleFireMode()
{
	// TODO : Refactor Toggle Fire Mode Logic
	if (!IsValid(CurrentEquippedItem))
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] Not Valid Current Equipped Item"), CURRENT_CONTEXT);
		return;
	}
	
	AFirableBase* CurrentFirable = Cast<AFirableBase>(CurrentEquippedItem);
	if (CurrentFirable)
	{
		const FFirableData Data = CurrentFirable->GetFirableData();
		for (int32 i = 0; i < Data.FireTypes.Num(); i++)
		{
			if (Data.FireTypes[i] == CurrentFireType)
				continue;
			
			CurrentFireType = Data.FireTypes[i];
			CurrentFirable->SetFireType(CurrentFireType);
			UE_LOG(LogTemp, Log, TEXT("[%s] Toggled Fire Mode to: %s"), CURRENT_CONTEXT, *EnumHelper::EnumToString(CurrentFireType));
			return;
		}
	}
}

void UEquipmentController::Equip(const TObjectPtr<AEquipmentBase>& Equipment)
{
	UnEquipCurrentEquipment();

	CurrentEquippedItem = Equipment;

	if (IsValid(CurrentEquippedItem))
	{
		const ACharacterPlayer* PlayerCharacter = Cast<ACharacterPlayer>(GetOwner());
		if (PlayerCharacter)
		{
			CurrentEquippedItem->SetActorHiddenInGame(false);
			CurrentEquippedItem->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("weapon_r"));
		}
		UE_LOG(LogTemp, Log, TEXT("[%s] Equipped: %s"), CURRENT_CONTEXT, *CurrentEquippedItem->GetName());
	}
}

void UEquipmentController::UnEquipCurrentEquipment()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->SetActorHiddenInGame(true);
	}
	CurrentEquippedItem = nullptr;
}
