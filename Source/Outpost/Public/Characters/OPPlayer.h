// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OPCharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "OPPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMovementDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractDelegate, FText, ObjectName, EInteractType, ObjectType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FItemPickupDelegate,  bool, bWeaponPickup, bool, bAmmoPickup, bool, bHealthPickup);

//Forward declarations.
class UInputAction;
class UInputMappingContext;
class UCameraComponent;

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

	//Overridden from OPCharacterBase class.
	virtual void CharacterDeath() override;

	/* Overridden from OPCharacterInterface */
	
	virtual void MeleeSphereTrace_Implementation(FVector MeleeStart, FVector MeleeEnd, float Radius) override;
	virtual void PickUpWeapon_Implementation(AOPWeapon* NewWeapon) override;
	virtual void PickUpAmmo_Implementation(EWeaponType AmmoType, int32 Amount) override;
	virtual bool IsPlayerReserveAmmoMaxedOut_Implementation(EWeaponType TypeToCheck) override;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Input|Actions|Weapons")
		TObjectPtr<UInputAction> ChangeFireModeAction;

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Timelines")
		TObjectPtr<UCurveFloat> ZoomCurve;

	/*
	Updates the keys in ZoomCurve, so that the weapon zoom mechanic works the same regardless of what the player's FOV settings are.
	In Blueprint, this should be called after the player changes their FOV settings, but before they resume playing.
	*/
	UFUNCTION(BlueprintCallable, Category = "OPPlayer|Aiming")
		void UpdateZoomCurveKeys();

	/* Inventory */

	//The class of the weapon that the player will spawn with, if applicable.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Inventory|Weapons")
		TSubclassOf<AOPWeapon> StartingWeaponClass;

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

	//The maximum amount of reserve ammo that the player can have of each type.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Inventory|Ammo")
		int32 MaxReserveAmmo = 999;

	/* Interaction */

	//The maximum distance at which the player can interact with applicable objects.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPPlayer|Interaction")
		float InteractRadius = 100.f;

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

	/* Sprinting */

	//The player's speed when they're moving normally.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Sprinting")
		float BaseSpeed = 600.f;

	//The player's speed when they're sprinting.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OPPlayer|Sprinting")
		float SprintSpeed = 900.f;

	/* Booleans */

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bCanPlayerInteract;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bIsPlayerCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bIsPlayerSprinting;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bIsPlayerZoomedIn;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bCanPlayerFire = true;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bCanPlayerReload = true;

	UPROPERTY(BlueprintReadOnly, Category = "OPPlayer|Booleans")
		bool bCanPlayerSwitch = true;

	/* Delegates */

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "OPPlayer|Delegates")
		FMovementDelegate OnMovementUpdate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "OPPlayer|Delegates")
		FInteractDelegate OnInteractUpdate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "OPPlayer|Delegates")
		FWeaponDelegate OnWeaponUpdate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "OPPlayer|Delegates")
		FItemPickupDelegate OnItemPickup;

	UPROPERTY()
		TObjectPtr<AActor> FocusedActor;

	UFUNCTION()
		void AdjustPlayerZoom(float Value);

	UFUNCTION()
		void EndSwitch(AOPWeapon* NewWeapon);

	UFUNCTION()
		void DebugReplenishReserveAmmo(EWeaponType AmmoType);
	
	void MoveForward(const FInputActionValue& Value);
	void MoveBackward(const FInputActionValue& Value);
	void StrafeLeft(const FInputActionValue& Value);
	void StrafeRight(const FInputActionValue& Value);
	void GamepadMove(const FInputActionValue& Value);

	void MouseLook(const FInputActionValue& Value);
	void GamepadLook(const FInputActionValue& Value);

	void StartJump();

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
	void TakeAmmoFromReserve(int32& ReserveAmmo);

	void CycleWeapons(const FInputActionValue& Value);
	void StartSwitch(AOPWeapon* NewWeapon);
	void HideAllUnequippedWeapons(AOPWeapon* NewWeapon);
	void AddPlayerTagsAfterWeaponSwitch();

	void ChangeFireMode();
	
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