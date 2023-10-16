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

void UOZEFunctionLibrary::CustomDebugMessage(const FString& Message, FColor TextColor, float DisplayTime)
{
	//Print the intended message onscreen.
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, DisplayTime, TextColor, Message);
	
	//Afterwards, print it to the log as well.
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}