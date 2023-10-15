/*
This is a personal Blueprint Function Library, for storing helper functions that I've created for myself.
It is designed to be project-agnostic, so implementation should be as simple as adding it to a project in the IDE.

Copyright (C) 2023 Okwudili Egwuatu. All Rights Reserved.
*/

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OZEFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class OZEHELPERPLUGIN_API UOZEFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/* Player controller functions */

	/*
	 A custom version of UWidgetBlueprintLibrary::SetInputMode_GameOnly(), that doesn't change the viewport mouse capture mode.
	 @param	PlayerIndex	The numerical index of the local or remote player.
	 @param	bConsumeCaptureMouseDown	Leave this "false" to avoid issues with mouse clicks.
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "OZEFunctionLibrary|Player Controller")
		static void CustomGameOnlyInputMode(UObject* WorldContextObject, int32 PlayerIndex, bool bConsumeCaptureMouseDown);
	
protected:
	
};

/* HOW TO USE THIS CLASS IN UNREAL PROJECTS:
1. Add the OZEFunctionLibrary.h and OZEFunctionLibrary.cpp files to the Unreal project's Source folder.

2. #include "OZEFunctionLibrary.h" in the desired class.

3. All functions in this Blueprint Function Library are static, so function calls must be prefixed with "UOZEFunctionLibrary::".
*/