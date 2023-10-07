// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OPInteractInterface.h"
#include "GameplayTagContainer.h"
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
	virtual FText GetInteractableObjectName_Implementation() override;
	virtual EInteractType GetInteractableObjectType_Implementation() override;

	/* Gameplay tags */

	//All of the gameplay tags that are currently applied to this weapon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OPWeapon")
		FGameplayTagContainer WeaponTags;

	/* Weapon stats */

	//The name of this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Info")
		FText WeaponName;

	//The category that this weapon belongs to.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Info")
		EWeaponType WeaponType;

	//The type of interactable object that this weapon is.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Weapon Stats|Info")
		EInteractType ObjectType;

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

	/* Infinite ammo */

	UFUNCTION()
		void CheckInfiniteAmmoStatus();

	void Shoot();
	void WeaponLineTrace();
	FVector CalculateWeaponSpread();

	FHitResult WeaponHitResult;
	
	//Out parameters for storing the player camera's location and rotation.
	FVector CameraLocation;
	FRotator CameraRotation;

	TObjectPtr<UOPWorldSubsystem> WorldSubsystem;

	int32 BurstCount;
};