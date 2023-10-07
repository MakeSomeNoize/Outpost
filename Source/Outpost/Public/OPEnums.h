/*
This class contains all of the custom enums that will be used in the Outpost project.
*/

#pragma once

#include "OPEnums.generated.h"

//Determines what category a particular weapon belongs to.
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE	UMETA(DisplayName = "NONE"),
	Pistol	UMETA(DisplayName = "Pistol"),
	Rifle	UMETA(DisplayName = "Rifle"),
	Shotgun	UMETA(DisplayName = "Shotgun"),
	Sniper	UMETA(DisplayName = "Sniper")
};

//Determines what category an interactable object belongs to.
UENUM(BlueprintType)
enum class EInteractType : uint8
{
	NONE	UMETA(DisplayName = "NONE"),
	Item	UMETA(DisplayName = "Item"),
	Switch	UMETA(DisplayName = "Switch")
};