// Copyright Epic Games, Inc. All Rights Reserved.

#include "LiquidXTestGameMode.h"
#include "Character/LiquidXTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALiquidXTestGameMode::ALiquidXTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
