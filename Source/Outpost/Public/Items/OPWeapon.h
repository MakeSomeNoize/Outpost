// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OPInteractInterface.h"
#include "OPStructs.h"
#include "OPWeapon.generated.h"

//Forward declarations.
class UOPWorldSubsystem;

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

	/* Actor and scene components */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPWeapon|Components")
		TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	/* Weapon stats */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPWeapon|Weapon Stats")
		FWeaponStats Stats;

	/* Cooldowns */

	UPROPERTY(BlueprintReadOnly, Category = "OPWeapon|Cooldowns")
		bool bFiringCooldownActive;

	UPROPERTY(BlueprintReadOnly, Category = "OPWeapon|Cooldowns")
		bool bAnimationCooldownActive;

	/* Animation */

	//The socket that this weapon will attach to, when picked up by a character.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation")
		FName AttachToSocket = FName("FPSPistol");

	//The montage that will play on a CHARACTER, when they fire this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation|Montages|Firing")
		TObjectPtr<UAnimMontage> CharacterFireMontage;

	//The montage that will play on this WEAPON, when it shoots.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation|Montages|Firing")
		TObjectPtr<UAnimMontage> WeaponShootMontage;

	//The montage that will play on a CHARACTER, when they reload this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation|Montages")
		TObjectPtr<UAnimMontage> CharacterReloadMontage;

	//The montage that will play on a CHARACTER, when they equip this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation|Montages|Switching")
		TObjectPtr<UAnimMontage> CharacterEquipMontage;

	//The montage that will play on a CHARACTER, when they unequip this weapon.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPWeapon|Animation|Montages|Switching")
		TObjectPtr<UAnimMontage> CharacterUnequipMontage;
	
	void Shoot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Actor and scene components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPWeapon|Components")
		TObjectPtr<USceneComponent> WeaponRoot;

	UFUNCTION()
		void CheckInfiniteAmmoStatus();
	
	void WeaponLineTrace();
	FVector CalculateWeaponSpread();

	void EndFiringCooldown();
	void EndAnimationCooldown();

	FTimerHandle FiringCooldownHandle;
	FTimerHandle AnimationCooldownHandle;

	FHitResult WeaponHitResult;
	
	//Out parameters for storing the player camera's location and rotation.
	FVector CameraLocation;
	FRotator CameraRotation;

	TObjectPtr<UOPWorldSubsystem> WorldSubsystem;

	int32 BurstCount;
};