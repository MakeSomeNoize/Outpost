﻿/*
This class contains all of the custom structs that will be used in the Outpost project.
*/

#pragma once

#include "OPEnums.h"
#include "OPStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	//The name of this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FText WeaponName = FText::FromString("Weapon");

	//The category that this weapon belongs to.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EWeaponType WeaponType;

	//The type of interactable object that this weapon is.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EInteractType ObjectType = EInteractType::Item;

	//The amount of damage that this weapon inflicts, per shot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 Damage = 1;

	//The type of damage that this weapon inflicts.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UDamageType> DamageType;

	//The furthest away that this weapon's rounds will travel.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxRange = 10000.f;

	//The frequency at which this weapon fires.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float FireRate = 1.f;

	//Determines whether this weapon is automatic, or not.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsWeaponAutomatic;

	//The frequency at which this weapon's bursts fire, if applicable.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BurstFireRate = 1.f;

	//Determines whether this weapon is burst-fire, or not.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsWeaponBurst;

	//The number of rounds that are currently in this weapon's magazine.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		int32 CurrentMagazine;

	//The maximum number of rounds that this weapon can hold in a magazine.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 MaxMagazine = 1;

	/*
	The degree to which this weapon's rounds will deviate from the player's crosshairs.
	This value should be set fairly low, between 0.01 and 0.05 in most cases.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float SpreadRadius = 0.05;
	
	//The number of shots that this weapon fires, when the trigger is pulled.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 ShotAmount = 1;
};