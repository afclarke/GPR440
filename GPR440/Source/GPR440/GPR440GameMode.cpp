// Copyright Epic Games, Inc. All Rights Reserved.

#include "GPR440GameMode.h"
#include "GPR440Character.h"
#include "UObject/ConstructorHelpers.h"

AGPR440GameMode::AGPR440GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
