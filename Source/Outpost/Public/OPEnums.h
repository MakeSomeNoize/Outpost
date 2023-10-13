/*
This class contains all of the custom enums that will be used in the Outpost project.
*/

#pragma once

#include "OPEnums.generated.h"

//Determines whether the input for a particular action is a "toggle" or a "hold".
UENUM(BlueprintType)
enum class EInputState : uint8
{
	Toggle	UMETA(DisplayName = "Toggle"),
	Hold	UMETA(DisplayName = "Hold")
};

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

//Determines what fire mode a weapon is currently using.
UENUM(BlueprintType)
enum class EFireMode : uint8
{
	SemiAuto	UMETA(DisplayName = "Semi"),
	Burst	UMETA(DisplayName = "Burst"),
	FullAuto	UMETA(DisplayName = "Auto")
};

//Determines what category an interactable object belongs to.
UENUM(BlueprintType)
enum class EInteractType : uint8
{
	NONE	UMETA(DisplayName = "NONE"),
	Item	UMETA(DisplayName = "Item"),
	Switch	UMETA(DisplayName = "Switch")
};