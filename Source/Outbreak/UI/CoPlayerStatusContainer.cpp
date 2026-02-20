// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CoPlayerStatusContainer.h"
#include "Components/VerticalBox.h"
#include "CoPlayerStatusWidget.h"
#include "Components/PanelWidget.h"

void UCoPlayerStatusContainer::AddPlayer(APlayerState* PS)
{
	if (!VB_List || !CoPlayerStatusClass) return;

	UCoPlayerStatusWidget* NewWidget = CreateWidget<UCoPlayerStatusWidget>(this, CoPlayerStatusClass);

	if (NewWidget)
	{
		NewWidget->Setup(PS);
		VB_List->AddChildToVerticalBox(NewWidget);
	}
}

void UCoPlayerStatusContainer::ClearList()
{
	if (VB_List)
	{
		VB_List->ClearChildren();
	}
}

void UCoPlayerStatusContainer::UpdateChildHealth(APlayerState* playerState, float ratio)
{
	if (!VB_List) return;

	int32 ChildCount = VB_List->GetChildrenCount();
    
	for (int32 i = 0; i < ChildCount; i++)
	{
		UWidget* Child = VB_List->GetChildAt(i);
        
		UCoPlayerStatusWidget* StatusWidget = Cast<UCoPlayerStatusWidget>(Child);
        
		if (StatusWidget)
		{
			if (StatusWidget->GetTargetPS() == playerState)
			{
				StatusWidget->UpdateHealthBar(ratio);
				break; 
			}
		}
	}
}
