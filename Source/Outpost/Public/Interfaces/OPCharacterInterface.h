// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Items/OPWeapon.h"
#include "OPCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOPCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OUTPOST_API IOPCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
public:
	/*
	Performs a sphere trace, to see if the character's melee attack hit anything. To be used in Animation Notifies only.
	@param	MeleeStart	The location where the sphere trace starts.
	@param	MeleeEnd	The location where the sphere trace ends.
	@param	Radius	The overall width of the melee hitbox.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPCharacterInterface|Melee")
		void MeleeSphereTrace(FVector MeleeStart, FVector MeleeEnd, float Radius);

	//Checks all actors that were hit by the character's last melee attack, and damages them if possible.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPCharacterInterface|Melee")
		void ProcessMeleeHitOnTargets();

	/*
	Picks up a weapon, and adds it to the player's inventory.
	@param	NewWeapon	A reference to the weapon that was just picked up.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPCharacterInterface|Inventory")
		void PickUpWeapon(AOPWeapon* NewWeapon);

	/*
	Picks up reserve ammo, and adds it to the player's inventory.
	@param	AmmoType	The category of weapon that the reserve ammo belongs to.
	@param	Amount	The amount of reserve ammo that was picked up.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPCharacterInterface|Inventory")
		void PickUpAmmo(EWeaponType AmmoType, int32 Amount);

	/*
	Returns "true" if the player has reached their limit for the specified type of reserve ammo, and "false" if they have not.
	@param	TypeToCheck	The category of weapon, whose reserve ammo should be checked.
	@return	Is the player unable to pick up any more reserve ammo?
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "OPCharacterInterface|Inventory")
		bool IsPlayerReserveAmmoMaxedOut(EWeaponType TypeToCheck);

protected:
	
};