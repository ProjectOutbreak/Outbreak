// Fill out your copyright notice in the Description page of Project Settings.


#include "OBHUD.h"
#include "OBWidget.h"
#include "Outbreak/Game/Framework/OBGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


void AOBHUD::BeginPlay()
{
    Super::BeginPlay();

    if (UOBGameInstance* GI = GetGameInstance<UOBGameInstance>())
    {
        if (TSubclassOf<UUserWidget> Cached = GI->GetCachedWidgetClass())
        {
            WidgetClass = Cached;
        }
    }
	
	APlayerController* PC = GetOwningPlayerController();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(this, 0);
    }

    if (WidgetClass && PC)
    {
        if (!OB_Widget)
        {
            OB_Widget = CreateWidget<UOBWidget>(PC, WidgetClass);
        }

        if (OB_Widget)
        {
            OB_Widget->AddToViewport();
            OB_Widget->SetVisibility(ESlateVisibility::Visible);
            OB_Widget->SetAnnouncementText("");
        }
    }
}

void AOBHUD::SetCutsceneMode(bool bEnable)
{
	if (OB_Widget)
	{
		OB_Widget->SetCutsceneMode(bEnable);
	}
}

void AOBHUD::DisplayAlivePlayerCount(int32 AlivePlayerCount)
{
	if (OB_Widget)
	{
		OB_Widget->SetAlivePlayerCountText(AlivePlayerCount);
		OB_Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AOBHUD::DisplayAnnouncementMessage(const FString& Message)
{
	if (OB_Widget)
	{
		OB_Widget->SetAnnouncementText(Message);
		OB_Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AOBHUD::DisplayTotalZombieKills(int32 TotalKills)
{
	if (OB_Widget)
	{
		OB_Widget->SetTotalZombieKillsText(TotalKills);
		OB_Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AOBHUD::DisplayZombieKills(int32 Kills)
{
	if (OB_Widget)
	{
		OB_Widget->SetZombieKillsText(Kills);
		OB_Widget->SetVisibility(ESlateVisibility::Visible);
	}
}
void AOBHUD::DisplayAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (OB_Widget)
	{
		OB_Widget->SetAmmoText(CurrentAmmo, TotalAmmo);
	}
}

void AOBHUD::DisplayWeaponType(FString Type)
{
	if (OB_Widget)
	{
		OB_Widget->SetWeaponTypeText(Type);
	}
}

void AOBHUD::DisplayCurrentHealth(int32 CurrentHealth)
{
	if (OB_Widget)
	{
		// TODO : PlayerData 에서 MaxHealth 가져오기
		float Percent = (float)CurrentHealth / 100.0f;
		OB_Widget->SetCurrentHealth(CurrentHealth, Percent);
	}
}