// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUD.h"
#include "OBWidget.h"
#include "Outbreak/Game/Framework/OutbreakGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OBCrouchDisplay.h"
#include "Blueprint/UserWidget.h"

void AInGameHUD::BeginPlay()
{
    Super::BeginPlay();

    if (UOutbreakGameInstance* GI = GetGameInstance<UOutbreakGameInstance>())
    {
        if (TSubclassOf<UUserWidget> Cached = GI->GetCachedWidgetClass())
        {
            InGameWidgetClass = Cached;
        }
    }
	
	APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(this, 0);
    }

    if (InGameWidgetClass && PC)
    {
        if (!OB_Widget)
        {
            OB_Widget = CreateWidget<UOBWidget>(PC, InGameWidgetClass);
        }

        if (OB_Widget)
        {
            OB_Widget->AddToViewport();
            OB_Widget->SetVisibility(ESlateVisibility::Visible);
            OB_Widget->SetAnnouncementText("");
        }
    }
}

void AInGameHUD::SetCutsceneMode(bool bEnable)
{
	if (OB_Widget)
	{
		OB_Widget->SetCutsceneMode(bEnable);
	}
}

void AInGameHUD::DisplayAlivePlayerCount(int32 AlivePlayerCount)
{
	// if (OB_Widget)
	// {
	// 	OB_Widget->SetAlivePlayerCountText(AlivePlayerCount);
	// 	OB_Widget->SetVisibility(ESlateVisibility::Visible);
	// }
}

void AInGameHUD::DisplayAnnouncementMessage(const FString& Message)
{
	if (OB_Widget)
	{
		OB_Widget->SetAnnouncementText(Message);
		OB_Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AInGameHUD::DisplayTotalZombieKills(int32 TotalKills)
{
	// if (OB_Widget)
	// {
	// 	OB_Widget->SetTotalZombieKillsText(TotalKills);
	// 	OB_Widget->SetVisibility(ESlateVisibility::Visible);
	// }
}

void AInGameHUD::DisplayZombieKills(int32 Kills)
{
	// if (OB_Widget)
	// {
	// 	OB_Widget->SetZombieKillsText(Kills);
	// 	OB_Widget->SetVisibility(ESlateVisibility::Visible);
	// }
}
void AInGameHUD::DisplayAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (OB_Widget)
	{
		OB_Widget->SetAmmoText(CurrentAmmo, TotalAmmo);
	}
}

void AInGameHUD::DisplayCurrentHealth(int32 CurrentHealth)
{
	if (OB_Widget)
	{
		// TODO : PlayerData 에서 MaxHealth 가져오기
		float Percent = (float)CurrentHealth / 100.0f;
		OB_Widget->SetCurrentHealth(CurrentHealth, Percent);
	}
}
void AInGameHUD::SetCrouchIcon(bool IsCrouch)
{
	if (OB_Widget)
	{
		OB_Widget->SetCrouchState(IsCrouch);
	}
}
