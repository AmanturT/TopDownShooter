// Copyright Epic Games, Inc. All Rights Reserved.

#include "TopDownShooteUE4GameMode.h"
#include "TopDownShooteUE4PlayerController.h"
#include "TopDownShooteUE4/Character/TopDownShooteUE4Character.h"
#include "UObject/ConstructorHelpers.h"

ATopDownShooteUE4GameMode::ATopDownShooteUE4GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATopDownShooteUE4PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	
}

void ATopDownShooteUE4GameMode::PlayerCharacterDead()
{
}
