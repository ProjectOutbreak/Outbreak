// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "OBWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void AInGameHUD::BeginPlay()
{
    Super::BeginPlay();
	
	CreateInGameWidget();
}

void AInGameHUD::SetCutsceneMode(bool bEnable)
{
	if (InGameWidgetInstance)
	{
		InGameWidgetInstance->SetCutsceneMode(bEnable);
	}
}

void AInGameHUD::DisplayAlivePlayerCount(int32 AlivePlayerCount)
{
	// if (InGameWidgetInstance)
	// {
	// 	InGameWidgetInstance->SetAlivePlayerCountText(AlivePlayerCount);
	// 	InGameWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	// }
}

void AInGameHUD::DisplayAnnouncementMessage(const FString& Message)
{
	if (InGameWidgetInstance)
	{
		InGameWidgetInstance->SetAnnouncementText(Message);
		InGameWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void AInGameHUD::DisplayTotalZombieKills(int32 TotalKills)
{
	// if (InGameWidgetInstance)
	// {
	// 	InGameWidgetInstance->SetTotalZombieKillsText(TotalKills);
	// 	InGameWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	// }
}

void AInGameHUD::DisplayZombieKills(int32 Kills)
{
	// if (InGameWidgetInstance)
	// {
	// 	InGameWidgetInstance->SetZombieKillsText(Kills);
	// 	InGameWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	// }
}
void AInGameHUD::DisplayAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (InGameWidgetInstance)
	{
		InGameWidgetInstance->SetAmmoText(CurrentAmmo, TotalAmmo);
	}
}

void AInGameHUD::DisplayCurrentHealth(int32 CurrentHealth)
{
	if (InGameWidgetInstance)
	{
		float Percent = (float)CurrentHealth / 100.0f;
		InGameWidgetInstance->SetCurrentHealth(CurrentHealth, Percent);
	}
}
void AInGameHUD::SetCrouchIcon(bool IsCrouch)
{
	if (InGameWidgetInstance)
	{
		InGameWidgetInstance->SetCrouchState(IsCrouch);
	}
}

void AInGameHUD::CreateInGameWidget()
{
	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		PC = UGameplayStatics::GetPlayerController(this, 0);
	}
	
	if (InGameWidgetClass && PC)
	{
		if (!InGameWidgetInstance)
		{
			InGameWidgetInstance = CreateWidget<UOBWidget>(PC, InGameWidgetClass);
		}

		if (InGameWidgetInstance)
		{
			InGameWidgetInstance->AddToViewport();
			InGameWidgetInstance->SetVisibility(ESlateVisibility::Visible);
			InGameWidgetInstance->SetAnnouncementText("");

			const FInputModeGameOnly InputModeData;
			PC->SetInputMode(InputModeData);
			PC->bShowMouseCursor = false;
		}
	}
	
	if (PerformanceWidgetClass && PC)
	{
		if (const TObjectPtr<UUserWidget> PerformanceWidgetInstance = CreateWidget<UUserWidget>(PC, PerformanceWidgetClass))
		{
			PerformanceWidgetInstance->AddToViewport();
		}
	}
}
void AInGameHUD::SetWeaponContainer(UTexture2D* Icon)
{
	if (InGameWidgetInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("위젯 인스턴스는 확인됨. 함수 진입 시도."));
		InGameWidgetInstance->SetWeaponContainer(Icon);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InGameWidgetInstance가 NULL입니다!"));
	}
}

void AInGameHUD::SetSubWeaponContainer(UTexture2D* Icon, int32 SlotNum)
{
	if (InGameWidgetInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("위젯 인스턴스는 확인됨. 함수 진입 시도."));
		InGameWidgetInstance->SetSubWeaponContainer(Icon,SlotNum);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InGameWidgetInstance가 NULL입니다!"));
	}
}

void AInGameHUD::SetBottomInv(UTexture2D* Icon, int32 SlotNum)
{
	if (InGameWidgetInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("위젯 인스턴스는 확인됨. 함수 진입 시도."));
		InGameWidgetInstance->SetBottomInvSlot(Icon,SlotNum);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InGameWidgetInstance가 NULL입니다!"));
	}
}


