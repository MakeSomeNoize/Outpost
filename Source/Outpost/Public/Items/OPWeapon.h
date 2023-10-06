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

UCLASS()
class OUTPOST_API AOPWeapon : public AActor, public IOPInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOPWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Overridden from OPInteractInterface */

	virtual void StartFocus_Implementation() override;
	virtual void EndFocus_Implementation() override;
	virtual void OnInteract_Implementation(AActor* CallingPlayer) override;
	virtual FText GetInteractMessage_Implementation() override;

	/* Weapon stats */

	//The category that this weapon belongs to.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Info")
		EWeaponType WeaponType = EWeaponType::Unarmed;

	//The message that should be shown when this weapon can be interacted with.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Info")
		FText InteractMessage = FText::FromString("Pick up Weapon");

	//The amount of damage that this weapon inflicts, per shot.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Damage")
		int32 Damage = 1;

	//The type of damage that this weapon inflicts.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Damage")
		TSubclassOf<UDamageType> DamageType;

	//The furthest away that this weapon's rounds will travel.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats")
		float MaxRange = 10000.f;

	//The frequency at which this weapon fires.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Rate of Fire")
		float FireRate = 1.f;

	//The number of rounds that are currently in this weapon's magazine.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Magazine")
		int32 CurrentMagazine;

	//The maximum number of rounds that this weapon can hold in a magazine.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Magazine")
		int32 MaxMagazine = 1;

	/*
	The degree to which this weapon's rounds will deviate from the player's crosshairs.
	This value should be set fairly low, between 0.01 and 0.05 in most cases.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Spread")
		float SpreadRadius = 0.05;
	
	//The number of shots that this weapon fires, when the trigger is pulled.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Spread")
		int32 ShotAmount = 1;
	
	//Determines if this weapon is automatic, or not.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Rate of Fire")
		bool bIsWeaponAutomatic;
	
	/*
	Determines if this weapon has an additional animation-based cooldown, or not.
	This should only apply to certain automatic weapons, and all burst-fire weapons.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Rate of Fire")
		bool bDoesWeaponHaveAnimationCooldown;
	
	//Determines if this weapon is burst-fire, or not.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Rate of Fire|Burst Fire")
		bool bIsWeaponBurst;

	//How fast each shot in this weapon's bursts will fire, if applicable.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Rate of Fire|Burst Fire")
		float BurstFireRate = 1.f;

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