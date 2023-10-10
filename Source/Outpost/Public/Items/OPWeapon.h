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
		TObjectPtr<UStaticMeshComponent> WeaponMesh;

	/* Weapon stats */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPWeapon|Weapon Stats")
		FWeaponStats Stats;

	/* Cooldowns */

	UPROPERTY(BlueprintReadOnly, Category = "OPWeapon|Cooldowns")
		bool bFiringCooldownActive;

	UPROPERTY(BlueprintReadOnly, Category = "OPWeapon|Cooldowns")
		bool bAnimationCooldownActive;
	
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