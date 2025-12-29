// Fill out your copyright notice in the Description page of Project Settings.


#include "OBCrouchDisplay.h"

#include "Components/Image.h"

void UOBCrouchDisplay::SetCrouchImage(bool IsCrouch)
{
	if (CrouchImage)
	{
		if (IsCrouch)
		{
			CrouchImage -> SetBrushFromTexture(CrouchIconTexture);
		}
		else
		{
			CrouchImage -> SetBrushFromTexture(StandIconTexture);
		}
	}

}
