// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//Determines what category a particular weapon belongs to.
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed	UMETA(DisplayName = "Unarmed"),
	Pistol	UMETA(DisplayName = "Pistol"),
	Rifle	UMETA(DisplayName = "Rifle"),
	Shotgun	UMETA(DisplayName = "Shotgun"),
	Sniper	UMETA(DisplayName = "Sniper")
};