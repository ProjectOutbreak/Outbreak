// Fill out your copyright notice in the Description page of Project Settings.


#include "BottomInv.h"
#include "Components/Image.h"

void UBottomInv::SetInvIcon(UTexture2D* Icon, int32 SlotNum)
{
	switch (SlotNum)
	{
		case 1:
			{
				if (IsValid(BottomInvSlot1))
				{
					if (Icon)
					{
						BottomInvSlot1->SetBrushFromTexture(Icon);
						BottomInvSlot1->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						BottomInvSlot1->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		case 2:
			{
				if (IsValid(BottomInvSlot2))
				{
					if (Icon)
					{
						BottomInvSlot2->SetBrushFromTexture(Icon);
						BottomInvSlot2->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						BottomInvSlot2->SetVisibility(ESlateVisibility::Hidden);
					}
				}
			}
		case 3:
		{
				if (IsValid(BottomInvSlot3))
				{
					if (Icon)
					{
						BottomInvSlot3->SetBrushFromTexture(Icon);
						BottomInvSlot3->SetVisibility(ESlateVisibility::Visible);
					}
					else
					{
						BottomInvSlot3->SetVisibility(ESlateVisibility::Hidden);
					}
				}
		}
		default:
		{
			
		}
	}
}