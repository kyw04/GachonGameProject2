// Copyright Epic Games, Inc. All Rights Reserved.

#include "GachonGameProject2GameMode.h"
#include "GachonGameProject2Character.h"
#include "UObject/ConstructorHelpers.h"

AGachonGameProject2GameMode::AGachonGameProject2GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
