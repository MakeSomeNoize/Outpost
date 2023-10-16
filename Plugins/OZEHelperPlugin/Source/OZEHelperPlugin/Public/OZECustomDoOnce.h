/*
This custom class replicates the functionality of the "Do Once" node in Blueprint.
It is designed to be project-agnostic, so implementation should be as simple as adding it to a project in the IDE.
NOTE: The functions in this custom class can only be directly used in C++!

Copyright (C) 2023 Okwudili Egwuatu. All Rights Reserved.
*/

#pragma once

#include "OZECustomDoOnce.generated.h"

USTRUCT(BlueprintType)
struct FDoOnce
{
	GENERATED_BODY()

	//Constructor declarations.
	FORCEINLINE FDoOnce();
	explicit FORCEINLINE FDoOnce(bool bStartClosed);

	//Checks to see if logic can be executed. Disables further executions afterward.
	FORCEINLINE bool Execute()
	{
		if (bDoOnce)
		{
			bDoOnce = false;
			return true;
		}

		return false;
	}

	//Re-enables the ability to execute logic.
	FORCEINLINE void Reset() { bDoOnce = true; }

	//Dictates whether or not logic can be executed.
	UPROPERTY(BlueprintReadWrite)
		bool bDoOnce;
};

//Constructor definitions.
FORCEINLINE FDoOnce::FDoOnce() : bDoOnce(false) {}
FORCEINLINE FDoOnce::FDoOnce(const bool bStartClosed) : bDoOnce(!bStartClosed) {}


/*
HOW TO USE THIS CLASS IN UNREAL PROJECTS:

1. Add the OZECustomDoOnce.h file to the Unreal project's Source folder.

2. #include "OZECustomDoOnce.h" in the desired class.

3. Declare a FDoOnce struct as follows:

UPROPERTY(VisibleAnywhere)
	  FDoOnce DoOnce = FDoOnce(false);

4. The bool parameter passed into the constructor determines whether the struct will start open, or start closed.

5. Use DoOnce.Execute() as a condition to check whether or not logic should be executed.

6. Call DoOnce.Reset() as needed, once logic has been executed.
*/