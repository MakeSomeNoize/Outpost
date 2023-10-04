// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OPInteractInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOPInteractInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OUTPOST_API IOPInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
public:
	/*
	Starts focus on the object.
	Typically this will do things like enable interaction for the player, show a UI prompt, highlight the object, etc.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPInteractInterface")
		void StartFocus();

	/*
	Ends focus on the object.
	Typically this will do things like disable interaction for the player, hide any UI prompts, un-highlight the object, etc.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPInteractInterface")
		void EndFocus();

	/*
	Executes interaction logic on the object.
	@param	CallingPlayer	A reference to the player that interacted with the object.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPInteractInterface")
		void OnInteract(AActor* CallingPlayer);

protected:
	
};