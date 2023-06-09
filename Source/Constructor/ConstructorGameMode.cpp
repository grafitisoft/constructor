// Copyright Epic Games, Inc. All Rights Reserved.

#include "ConstructorGameMode.h"
#include "ConstructorCharacter.h"
#include "UObject/ConstructorHelpers.h"

AConstructorGameMode::AConstructorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	
}
