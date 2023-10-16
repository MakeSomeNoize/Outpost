// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Subsystems/OPWorldSubsystem.h"
#include "Interfaces/OPCharacterInterface.h"
#include "OPCharacterBase.generated.h"

UCLASS()
class OUTPOST_API AOPCharacterBase : public ACharacter, public IOPCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Overridden from OPCharacterInterface */
	
	virtual void ProcessMeleeHitOnTargets_Implementation() override;

	/* Health */

	//Returns the character's current health.
	UFUNCTION(BlueprintPure, Category = "OPCharacterBase|Health")
		FORCEINLINE int32 GetCurrentHealth() { return CurrentHealth; }

	/*
	Changes the character's current health.
	@param	NewValue	The value that CurrentHealth should be changed to.
	*/
	UFUNCTION(BlueprintCallable, Category = "OPCharacterBase|Health")
		void SetCurrentHealth(int32 NewValue);

	//Returns the character's current health.
	UFUNCTION(BlueprintPure, Category = "OPCharacterBase|Health")
		FORCEINLINE int32 GetMaxHealth() { return MaxHealth; }

	/*
	Changes the character's current health.
	@param	NewValue	The value that MaxHealth should be changed to.
	*/
	UFUNCTION(BlueprintCallable, Category = "OPCharacterBase|Health")
		void SetMaxHealth(int32 NewValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Inventory */

	//All of the weapons that the character is carrying.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPCharacterBase|Inventory|Weapons")
		TArray<TObjectPtr<AOPWeapon>> WeaponArray;

	//The weapon that the character currently has equipped.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPCharacterBase|Inventory|Weapons")
		TObjectPtr<AOPWeapon> CurrentWeapon;

	//The category that the character's currently-equipped weapon belongs to.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPCharacterBase|Inventory|Weapons")
		EWeaponType CurrentWeaponType;

	/* Health */

	//The amount of health that the character currently has.
	UPROPERTY(VisibleInstanceOnly, Category = "OPCharacterBase|Health")
		int32 CurrentHealth = 1;

	//The maximum amount of health that the character can have.
	UPROPERTY(EditDefaultsOnly, Category = "OPCharacterBase|Health")
		int32 MaxHealth = 100;

	/* Booleans */

	UPROPERTY(BlueprintReadOnly, Category = "OPCharacterBase|Booleans")
		bool bIsCharacterDead;

	/* Melee */

	//The amount of damage that the player's melee attacks inflict.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPCharacterBase|Melee")
		int32 MeleeDamage = 1;

	//An array of all the hit results gathered by the character's last melee attack.
	UPROPERTY(BlueprintReadOnly, Category = "OPCharacterBase|Melee")
		TArray<FHitResult> MeleeHitResults;

	//The type of damage inflicted by this character's melee attacks.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPCharacterBase|Melee")
		TSubclassOf<UDamageType> MeleeDamageType;

	/*
	An array of all the actors damaged by the character's last melee attack.
	Actors should never be damaged more than once, by the same melee attack.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "OPCharacterBase|Melee")
		TArray<TObjectPtr<AActor>> ActorsDamaged;
	
	UPROPERTY()
		TObjectPtr<UOPWorldSubsystem> WorldSubsystem;

	virtual void CharacterDeath();
};