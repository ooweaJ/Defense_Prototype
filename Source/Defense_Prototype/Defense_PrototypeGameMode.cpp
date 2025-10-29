// Copyright Epic Games, Inc. All Rights Reserved.

#include "Defense_PrototypeGameMode.h"
#include "Defense_PrototypePlayerController.h"
#include "Defense_PrototypeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADefense_PrototypeGameMode::ADefense_PrototypeGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ADefense_PrototypePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}