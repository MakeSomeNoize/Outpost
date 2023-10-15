/*
This custom class replicates the functionality of the "Gate" node in Blueprint.
It is designed to be project-agnostic, so implementation should be as simple as adding it to a project in the IDE.
NOTE: The functions in this custom class can only be directly used in C++!

Copyright (C) 2023 Okwudili Egwuatu. All Rights Reserved.
*/

#pragma once

#include "OZECustomGate.generated.h"

USTRUCT(BlueprintType)
struct FGate
{
	GENERATED_BODY()

public:
	//Constructor declarations.
	FORCEINLINE FGate();
	explicit FORCEINLINE FGate(bool bStartClosed);

	//Opens the gate, and allows logic to be executed.
	FORCEINLINE void Open() { bGateOpen = true; }

	//Closes the gate, and prevents logic from being executed.
	FORCEINLINE void Close() { bGateOpen = false; }

	//Alternates between allowing and preventing logic execution.
	FORCEINLINE void Toggle() { bGateOpen = !bGateOpen; }

	//Checks to see if logic can be executed.
	FORCEINLINE bool IsOpen() { return bGateOpen; }

protected:
	//Dictates whether or not logic can be executed from the custom gate.
	UPROPERTY(VisibleAnywhere)
		bool bGateOpen;
};

//Constructor definitions.
FORCEINLINE FGate::FGate() : bGateOpen(false) {}
FORCEINLINE FGate::FGate(const bool bStartClosed) : bGateOpen(!bStartClosed) {}

/* HOW TO USE THIS CLASS IN UNREAL PROJECTS:
1. Add the OZECustomGate.h file to the Unreal project's Source folder.

2. #include "OZECustomGate.h" in the desired class.

3. Declare an FGate struct as follows:

UPROPERTY(VisibleAnywhere)
  	FGate Gate = FGate(false);

4. The bool parameter passed into the constructor determines whether the custom gate will start open, or start closed.

5. Use Gate.IsOpen() as a condition to check whether or not logic should be executed.

6. Call Gate.Open(), Gate.Close(), or Gate.Toggle() as needed.
*/