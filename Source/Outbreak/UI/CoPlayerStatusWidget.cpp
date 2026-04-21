// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CoPlayerStatusWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerState.h"

void UCoPlayerStatusWidget::Setup(APlayerState* PS)
{
	if (!PS) return;

	TargetPS = PS;

	if (User_Name)
	{
		User_Name->SetText(FText::FromString(PS->GetPlayerName()));
	}
	
	HealthBar->SetPercent(100.0f);
}

void UCoPlayerStatusWidget::SetNickname(FString NewName)
{
	if (User_Name)
	{
		UE_LOG(LogTemp, Warning, TEXT("닉네임 변경 성공! : %s"), *NewName);
		User_Name->SetText(FText::FromString(NewName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("오류: User_Name 텍스트 블록을 못 찾았습니다! BindWidget 확인하세요."));
	}
}

void UCoPlayerStatusWidget::UpdateHealthBar(float Ratio)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Ratio);
	}
}
