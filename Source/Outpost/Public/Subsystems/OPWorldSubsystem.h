// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "OPEnums.h"
#include "OPWorldSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInfiniteAmmoWithReloadDelegate, EWeaponType, CurrentWeaponType);

/**
 * 
 */
UCLASS()
class OUTPOST_API UOPWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Sets default values for this subsystem's properties
	UOPWorldSubsystem();

	// USubsystem implementation Begin
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* Debug options */
	
	//Determines whether debug lines for interact traces are visible, or not. 
	UPROPERTY(BlueprintReadWrite, Category = "OPWorldSubsystem|Debug Lines")
		bool bInteractDebugLinesEnabled;
		
	//Determines whether debug lines for weapon traces are visible, or not. 
	UPROPERTY(BlueprintReadWrite, Category = "OPWorldSubsystem|Debug Lines")
		bool bWeaponDebugLinesEnabled;

	//Determines whether debug lines for melee traces are visible, or not. 
	UPROPERTY(BlueprintReadWrite, Category = "OPWorldSubsystem|Debug Lines")
		bool bMeleeDebugLinesEnabled;

	/*
	Determines if the player's weapons have infinite ammo, or not.
	This version prevents the weapons' magazines from going empty.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "OPWorldSubsystem|Infinite Ammo")
		bool bInfiniteAmmoEnabled;
		
	/*
	Determines if the player's weapons have infinite ammo, or not.
	This version prevents the player's reserve ammo from going empty.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "OPWorldSubsystem|Infinite Ammo")
		bool bInfiniteAmmoWithReloadEnabled;

	/* Enemies */

	//An array of references to all enemies that are currently alive.
	UPROPERTY(BlueprintReadOnly, Category = "OPWorldSubsystem|Enemies")
		TArray<TObjectPtr<AActor>> EnemyArray;

	/* Delegates */

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "OPWordlSubsystem|Delegates")
		FInfiniteAmmoWithReloadDelegate OnInfiniteAmmoWithReloadUpdate;

protected:
	
};