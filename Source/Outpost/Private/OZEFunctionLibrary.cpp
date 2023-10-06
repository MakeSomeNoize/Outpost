// Fill out your copyright notice in the Description page of Project Settings.

#include "OZEFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UOZEFunctionLibrary::CustomGameOnlyInputMode(UObject* WorldContextObject, int32 PlayerIndex, bool bConsumeCaptureMouseDown)
{
	//Get a reference to the desired player controller.
	const TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);

	if (IsValid(PlayerController))
	{
		//Create a version of "Input Mode Game Only" that properly captures mouse input.
		FInputModeGameOnly InputMode;
		InputMode.SetConsumeCaptureMouseDown(bConsumeCaptureMouseDown);

		//Set the new input mode on the desired player controller.
		PlayerController->SetInputMode(InputMode);
	}
}