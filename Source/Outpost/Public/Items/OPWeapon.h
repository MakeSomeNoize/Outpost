// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Outpost/Outpost.h"
#include "Interfaces/OPInteractInterface.h"
#include "OPWeapon.generated.h"

//Forward declarations.
class UOPWorldSubsystem;
class UBoxComponent;

//A struct for the core attributes of a weapon.
USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()
	
	//The category that this weapon belongs to.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats")
		EWeaponType WeaponType;

	//The amount of damage that this weapon inflicts, per shot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Damage")
		int32 Damage;

	//The type of damage that this weapon inflicts.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Damage")
		TSubclassOf<UDamageType> DamageType;

	//The furthest away that this weapon's rounds will travel.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats")
		float MaxRange;

	//The frequency at which this weapon fires.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats")
		float FireRate;

	//The number of rounds that are currently in this weapon's magazine.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Magazine")
		int32 CurrentMagazine;

	//The maximum number of rounds that this weapon can hold in a magazine.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Magazine")
		int32 MaxMagazine;

	/*
	The degree to which this weapon's rounds will deviate from the player's crosshairs.
	This value should be set fairly low, between 0.01 and 0.05 in most cases.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Spread")
		float SpreadRadius;

	/*
	The number of shots that this weapon fires, when the trigger is pulled.
	This value should always remain 1, unless the weapon is a shotgun.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Spread")
		int32 ShotAmount;

	//Determines if this weapon is automatic, or not.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Booleans")
		bool bIsWeaponAutomatic;

	/*
	Determines if this weapon has an additional animation-based cooldown, or not.
	This only applies to automatic weapons that should NOT be allowed to fire as fast as the player can pull the trigger.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Booleans")
		bool bDoesWeaponHaveAnimationCooldown;

	//Default values for FWeaponStats struct.
	FWeaponStats() : WeaponType(EWeaponType::Unarmed), Damage(1), DamageType(nullptr), MaxRange(10000.f), FireRate(1.f), CurrentMagazine(0), MaxMagazine(1), SpreadRadius(0.05f), ShotAmount(1), bIsWeaponAutomatic(false), bDoesWeaponHaveAnimationCooldown(false) {}
};

UCLASS()
class OUTPOST_API AOPWeapon : public AActor, public IOPInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOPWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Overridden from IOPInteractInterface */

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void OnInteract_Implementation(AActor* CallingPlayer) override;

	//The core attributes of this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon")
		FWeaponStats Stats;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Actor and scene components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPWeapon|Components")
		TObjectPtr<USceneComponent> WeaponRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPWeapon|Components")
		TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPWeapon|Components")
		TObjectPtr<UBoxComponent> InteractRadius;

	void Shoot();
	void WeaponLineTrace();
	FVector CalculateWeaponSpread();

	FHitResult WeaponHitResult;
	
	//Out parameters for storing the player camera's location and rotation.
	FVector CameraLocation;
	FRotator CameraRotation;

	TObjectPtr<UOPWorldSubsystem> WorldSubsystem;
};