// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OPCharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "OPPlayer.generated.h"

//Forward declarations.
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class AOPWeapon;

/**
 * 
 */
UCLASS()
class OUTPOST_API AOPPlayer : public AOPCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPPlayer(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Required for EnhancedInput plugin.
	virtual void PawnClientRestart() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Actor and scene components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPPlayer|Components")
		TObjectPtr<UCameraComponent> PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPPlayer|Components")
		TObjectPtr<UTimelineComponent> ZoomTimeline;

	/* Enhanced Input actions and mapping contexts */

	//The IMC that holds all of the player inputs EXCEPT for "Hold Zoom", "Hold Crouch", and "Hold Sprint".
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input")
		TObjectPtr<UInputMappingContext> BaseContext;

	//The IMC that ONLY holds the "Hold Zoom", "Hold Crouch", and "Hold Sprint" inputs.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input")
		TObjectPtr<UInputMappingContext> ExtraContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> MoveBackwardAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> StrafeLeftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> StrafeRightAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> GamepadMoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Movement")
		TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Aiming")
		TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Aiming")
		TObjectPtr<UInputAction> GamepadLookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Weapons")
		TObjectPtr<UInputAction> FireWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Weapons")
		TObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Weapons")
		TObjectPtr<UInputAction> CycleWeaponsAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Interaction")
		TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Interaction")
		TObjectPtr<UInputAction> MeleeAction;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> ToggleZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> HoldZoomAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> ToggleCrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> HoldCrouchAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> ToggleSprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Toggle/Hold")
		TObjectPtr<UInputAction> HoldSprintAction;

	/* Curve assets for timelines */

	//The Curve asset that will be used for the weapon zoom mechanic.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer")
		TObjectPtr<UCurveFloat> ZoomCurve;

	/*
	Updates the keys in ZoomCurve, so that the weapon zoom mechanic works the same regardless of what the player's FOV settings are.
	In Blueprint, this should be called after the player changes their FOV settings, but before they resume playing.
	*/
	UFUNCTION(BlueprintCallable, Category = "OPPlayer|Aiming")
		void UpdateZoomCurveKeys();

	/* Player inventory */

	//All of the weapons that the player is currently carrying.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|Inventory|Weapons")
		TArray<TObjectPtr<AOPWeapon>> WeaponArray;

	//The weapon that the player currently has equipped.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|Inventory|Weapons")
		TObjectPtr<AOPWeapon> CurrentWeapon;

	//The weapons that the player will spawn with, if applicable.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Inventory|Weapons")
		TArray<TObjectPtr<AOPWeapon>> StartingWeapons;

	//The amount of pistol ammo that the player has in reserve.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Inventory|Ammo")
		int32 PistolAmmo;

	//The amount of rifle ammo that the player has in reserve.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Inventory|Ammo")
		int32 RifleAmmo;

	//The amount of shotgun ammo that the player has in reserve.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Inventory|Ammo")
		int32 ShotgunAmmo;

	//The amount of sniper ammo that the player has in reserve.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Inventory|Ammo")
		int32 SniperAmmo;

	/* Interaction */

	//The maximum distance at which the player can interact with applicable objects.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Interaction")
		float InteractRadius = 100.f;

	//The amount of damage that the player's melee attacks inflict.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Interaction")
		int32 MeleeDamage = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|Interaction")
		bool bCanPlayerInteract = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|Interaction")
		bool bCanPlayerMelee = true;

	/*
	Performs a sphere trace, to see if the player's melee attack hit anything.
	To be used in Animation Notifies only.
	*/
	UFUNCTION(BlueprintCallable, Category = "OPPlayer|Melee")
		void MeleeSphereTrace();

	/* General booleans */

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|General Booleans")
		bool bIsPlayerSprinting;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|General Booleans")
		bool bIsPlayerZoomedIn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|General Booleans")
		bool bCanPlayerFire = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|General Booleans")
		bool bCanPlayerReload = true;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPPlayer|General Booleans")
		bool bCanPlayerSwitch = true;

	/* Look sensitivity */

	//Determines how sensitive the mouse is to player input.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Look Sensitivity")
		float MouseSensitivity = 1.f;

	//Determines how sensitive analog sticks are to player input.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Look Sensitivity")
		float GamepadSensitivity = 100.f;

	/* Extra input behavior */

	//Determines if the player will need to hold the "zoom" input, or not.
	UPROPERTY(BlueprintReadWrite, Category = "OPPlayer|Extra Input Behavior")
		bool bHoldZoomEnabled;

	//Determines if the player will need to hold the "crouch" input, or not.
	UPROPERTY(BlueprintReadWrite, Category = "OPPlayer|Extra Input Behavior")
		bool bHoldCrouchEnabled;

	//Determines if the player will need to hold the "sprint" input, or not.
	UPROPERTY(BlueprintReadWrite, Category = "OPPlayer|Extra Input Behavior")
		bool bHoldSprintEnabled;

	//Changes the "zoom", "crouch", and/or "sprint" input behaviors from a toggle to a hold.
	UFUNCTION(BlueprintCallable, Category = "OPPlayer|Extra Input Behavior")
		void BindExtraInputBehaviorToPlayer();

	//The player's speed when they're moving normally.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Sprinting")
		float BaseSpeed = 600.f;

	//The player's speed when they're sprinting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Sprinting")
		float SprintSpeed = 900.f;

	UPROPERTY()
		TObjectPtr<AActor> FocusedActor;

	UFUNCTION()
		void AdjustPlayerZoom(float Value);
	
	void MoveForward(const FInputActionValue& Value);
	void MoveBackward(const FInputActionValue& Value);
	void StrafeLeft(const FInputActionValue& Value);
	void StrafeRight(const FInputActionValue& Value);
	void GamepadMove(const FInputActionValue& Value);

	void MouseLook(const FInputActionValue& Value);
	void GamepadLook(const FInputActionValue& Value);

	void ToggleZoom();
	void StartZoom();
	void StopZoom();

	void ToggleCrouch();
	void StartCrouch();
	void StopCrouch();

	void ToggleSprint();
	void StartSprint();
	void StopSprint();

	void StartFire();
	void FireWeapon();
	void StopFire();

	void StartReload();
	void EndReload();
	void TakeAmmoFromReserve(int32& AmmoToTake);

	void CycleWeapons(const FInputActionValue& Value);
	void StartSwitch();
	void EndSwitch();
	
	void Interact();
	void InteractLineTrace();
	void CheckForInteractableObjects();

	void StartMelee();
	void StopMelee();

	FTimerHandle FireHandle;
	FTimerHandle ReloadHandle;
	FTimerHandle SwitchHandle;
	FTimerHandle MeleeHandle;

	FHitResult InteractHitResult;
	FHitResult MeleeHitResult;

	//Out parameters for storing the player camera's location and rotation.
	FVector CameraLocation;
	FRotator CameraRotation;

	FOnTimelineFloat ZoomFunction{};
};