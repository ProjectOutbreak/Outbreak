// Fill out your copyright notice in the Description page of Project Settings.


#include "BottomInv.h"
#include "Components/Image.h"

void UBottomInv::SetInvIcon(UTexture2D* Icon, int32 SlotNum)
{
	UImage* TargetSlot = nullptr;

	switch (SlotNum)
	{
	case 1: TargetSlot = BottomInvSlot1; break;
	case 2: TargetSlot = BottomInvSlot2; break;
	case 3: TargetSlot = BottomInvSlot3; break;
	default: return;
	}
	
	if (IsValid(TargetSlot))
	{
		if (Icon)
		{
			TargetSlot->SetBrushFromTexture(Icon);
			TargetSlot->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			TargetSlot->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BottomInvSlot %d 가 유효하지 않습니다!"), SlotNum);
	}
}