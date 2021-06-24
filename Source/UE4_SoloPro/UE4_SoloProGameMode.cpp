// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE4_SoloProGameMode.h"
#include "UE4_SoloProCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE4_SoloProGameMode::AUE4_SoloProGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
