// Copyright Epic Games, Inc. All Rights Reserved.

#include "Test2GameMode.h"
#include "Test2Character.h"
#include "UObject/ConstructorHelpers.h"

ATest2GameMode::ATest2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
