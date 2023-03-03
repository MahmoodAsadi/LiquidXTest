// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/UW_Interaction.h"


void UUW_Interaction::SetInteractionWidgetEnable(bool bEnable)
{
	if (bEnable == bInteractionEnable)
	{
		// Do nothing if nothing changed.
		return;
	}

	bInteractionEnable = bEnable;
	BP_SetInteractionWidgetEnable(bEnable);
}

void UUW_Interaction::SetInteractionEnable_Implementation(bool bEnable)
{
	SetInteractionWidgetEnable(bEnable);
}