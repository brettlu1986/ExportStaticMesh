// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportStaticMeshGameMode.h"
#include "ExportStaticMeshCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExportStaticMeshGameMode::AExportStaticMeshGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
