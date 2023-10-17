// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/OPInteractInterface.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AOPPlayer::AOPPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true;

	//The character mesh should be attached to the camera, for first-person games.
	GetMesh()->SetupAttachment(PlayerCamera);

	ZoomTimeline = CreateDefaultSubobject<UTimelineComponent>("Zoom Timeline");
}

// Called when the game starts or when spawned
void AOPPlayer::BeginPlay()
{
	Super::BeginPlay();

	//Ensure that the player's zoom levels will be correct, and bind a callback function to the zoom timeline.
	if (IsValid(ZoomCurve))
	{
		UpdateZoomCurveKeys();

		ZoomFunction.BindUFunction(this, TEXT("AdjustPlayerZoom"));
		ZoomTimeline->AddInterpFloat(ZoomCurve, ZoomFunction);
	}

	GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;

	//Places a dummy "weapon" in the player's inventory, so WeaponArray will never be empty.
	WeaponArray.Emplace(NewObject<AOPWeapon>());

	if (IsValid(StartingWeaponClass))
	{
		FAttachmentTransformRules StartingRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
		
		TObjectPtr<AOPWeapon> StartingWeapon = GetWorld()->SpawnActor<AOPWeapon>(StartingWeaponClass);

		/*
		The player is set as the starting weapon's owner, it gets attached to their mesh...
		...The starting weapon's mesh no longer collides with pawns, or casts shadows...
		...Finally, it is added to the player's weapon array, and set as their current weapon.
		*/
		StartingWeapon->SetOwner(this);
		StartingWeapon->AttachToComponent(GetMesh(), StartingRules, StartingWeapon->AttachToSocket);
		StartingWeapon->WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		StartingWeapon->WeaponMesh->SetCastShadow(false);
		WeaponArray.Emplace(StartingWeapon);
		CurrentWeapon = StartingWeapon;
		CurrentWeaponType = CurrentWeapon->Stats.WeaponType;
	}
	else
	{
		CurrentWeapon = WeaponArray[0];
	}

	//Bind a callback function to OnInfiniteAmmo delegate.
	if (IsValid(WorldSubsystem)) WorldSubsystem->OnInfiniteAmmoWithReloadUpdate.AddDynamic(this, &AOPPlayer::ReplenishReserveAmmo);

	//Bind a callback function to OnTakePointDamage delegate.
	OnTakePointDamage.AddDynamic(this, &AOPPlayer::TakePointDamage);
	
}

// Called every frame
void AOPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	InteractLineTrace();
	
}

// Called to bind functionality to input
void AOPPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IsValid(MoveForwardAction)) PlayerEnhancedInputComponent->BindAction(MoveForwardAction, ETriggerEvent::Triggered, this, &AOPPlayer::MoveForward);
		if (IsValid(MoveBackwardAction)) PlayerEnhancedInputComponent->BindAction(MoveBackwardAction, ETriggerEvent::Triggered, this, &AOPPlayer::MoveBackward);
		if (IsValid(StrafeLeftAction)) PlayerEnhancedInputComponent->BindAction(StrafeLeftAction, ETriggerEvent::Triggered, this, &AOPPlayer::StrafeLeft);
		if (IsValid(StrafeRightAction)) PlayerEnhancedInputComponent->BindAction(StrafeRightAction, ETriggerEvent::Triggered, this, &AOPPlayer::StrafeRight);
		if (IsValid(GamepadMoveAction)) PlayerEnhancedInputComponent->BindAction(GamepadMoveAction, ETriggerEvent::Triggered, this, &AOPPlayer::GamepadMove);

		if (IsValid(JumpAction)) PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);

		if (IsValid(MouseAndKeyboardToggleSprintAction)) PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardToggleSprintAction, ETriggerEvent::Triggered, this, &AOPPlayer::MouseAndKeyboardToggleSprint);
		if (IsValid(GamepadToggleSprintAction)) PlayerEnhancedInputComponent->BindAction(GamepadToggleSprintAction, ETriggerEvent::Triggered, this, &AOPPlayer::GamepadToggleSprint);

		if (IsValid(MouseAndKeyboardHoldSprintAction))
		{
			PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardHoldSprintAction, ETriggerEvent::Started, this, &AOPPlayer::MouseAndKeyboardStartSprint);
			PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardHoldSprintAction, ETriggerEvent::Completed, this, &AOPPlayer::MouseAndKeyboardStopSprint);
		}

		if (IsValid(GamepadHoldSprintAction))
		{
			PlayerEnhancedInputComponent->BindAction(GamepadHoldSprintAction, ETriggerEvent::Started, this, &AOPPlayer::GamepadStartSprint);
			PlayerEnhancedInputComponent->BindAction(GamepadHoldSprintAction, ETriggerEvent::Completed, this, &AOPPlayer::GamepadStopSprint);
		}

		if (IsValid(MouseLookAction)) PlayerEnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AOPPlayer::MouseLook);
		if (IsValid(GamepadLookAction)) PlayerEnhancedInputComponent->BindAction(GamepadLookAction, ETriggerEvent::Triggered, this, &AOPPlayer::GamepadLook);

		if (IsValid(MouseAndKeyboardToggleZoomAction)) PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardToggleZoomAction, ETriggerEvent::Triggered, this, &AOPPlayer::MouseAndKeyboardToggleZoom);
		if (IsValid(GamepadToggleZoomAction)) PlayerEnhancedInputComponent->BindAction(GamepadToggleZoomAction, ETriggerEvent::Triggered, this, &AOPPlayer::GamepadToggleZoom);

		if (IsValid(MouseAndKeyboardHoldZoomAction))
		{
			PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardHoldZoomAction, ETriggerEvent::Started, this, &AOPPlayer::MouseAndKeyboardStartZoom);
			PlayerEnhancedInputComponent->BindAction(MouseAndKeyboardHoldZoomAction, ETriggerEvent::Completed, this, &AOPPlayer::MouseAndKeyboardStopZoom);
		}

		if (IsValid(GamepadHoldZoomAction))
		{
			PlayerEnhancedInputComponent->BindAction(GamepadHoldZoomAction, ETriggerEvent::Started, this, &AOPPlayer::GamepadStartZoom);
			PlayerEnhancedInputComponent->BindAction(GamepadHoldZoomAction, ETriggerEvent::Completed, this, &AOPPlayer::GamepadStopZoom);
		}

		if (IsValid(FireWeaponAction))
		{
			PlayerEnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AOPPlayer::StartFire);
			PlayerEnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AOPPlayer::StopFire);
		}

		if (IsValid(ReloadWeaponAction)) PlayerEnhancedInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Triggered, this, &AOPPlayer::StartReload);

		if (IsValid(CycleWeaponsAction)) PlayerEnhancedInputComponent->BindAction(CycleWeaponsAction, ETriggerEvent::Triggered, this, &AOPPlayer::CycleWeapons);

		if (IsValid(ChangeFireModeAction)) PlayerEnhancedInputComponent->BindAction(ChangeFireModeAction, ETriggerEvent::Triggered, this, &AOPPlayer::ChangeFireMode);
		
		if (IsValid(InteractAction)) PlayerEnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AOPPlayer::Interact);

		if (IsValid(MeleeAction)) PlayerEnhancedInputComponent->BindAction(MeleeAction, ETriggerEvent::Triggered, this, &AOPPlayer::StartMelee);
	}
}

void AOPPlayer::PawnClientRestart()
{
	Super::PawnClientRestart();

	// Make sure that we have a valid PlayerController.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();

			// Add each mapping context, along with their priority values. Higher values out-prioritize lower values.
			if (IsValid(DefaultContext)) Subsystem->AddMappingContext(DefaultContext, 0);
		}
	}
}

void AOPPlayer::MoveForward(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f) AddMovementInput(GetActorForwardVector(), Value[1]);
}

void AOPPlayer::MoveBackward(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f) AddMovementInput(GetActorForwardVector(), Value[1]);
}

void AOPPlayer::StrafeLeft(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f) AddMovementInput(GetActorRightVector(), Value[0]);
}

void AOPPlayer::StrafeRight(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f) AddMovementInput(GetActorRightVector(), Value[0]);
}

void AOPPlayer::GamepadMove(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f)
	{
		AddMovementInput(GetActorForwardVector(), Value[1]);
		AddMovementInput(GetActorRightVector(), Value[0]);
	}
}

void AOPPlayer::MouseLook(const FInputActionValue& Value)
{
	if (Value.GetMagnitude() != 0.f)
	{
		AddControllerPitchInput(Value[1] * MouseSensitivity);
		AddControllerYawInput(Value[0] * MouseSensitivity);
	}
}

void AOPPlayer::GamepadLook(const FInputActionValue& Value)
{
	//Analog stick input has to be multiplied by Delta Time, in order to be framerate-independent.
	if (Value.GetMagnitude() != 0.f)
	{
		AddControllerPitchInput(Value[1] * GamepadSensitivity * GetWorld()->GetDeltaSeconds());
		AddControllerYawInput(Value[0] * GamepadSensitivity * GetWorld()->GetDeltaSeconds());
	}
}

void AOPPlayer::MouseAndKeyboardToggleSprint()
{
	//Make sure the input state is actually set to a "toggle".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardSprintState != EInputState::Toggle) return;

	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is zoomed in, then sprinting will cause them to stop.
		UniversalStopZoom();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::GamepadToggleSprint()
{
	//Make sure the input state is actually set to a "toggle".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadSprintState != EInputState::Toggle) return;

	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is zoomed in, then sprinting will cause them to stop.
		UniversalStopZoom();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::MouseAndKeyboardStartSprint()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardSprintState != EInputState::Hold) return;

	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is zoomed in, then sprinting will cause them to stop.
		UniversalStopZoom();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::MouseAndKeyboardStopSprint()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardSprintState != EInputState::Hold) return;

	if (bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::GamepadStartSprint()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadSprintState != EInputState::Hold) return;

	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is zoomed in, then sprinting will cause them to stop.
		UniversalStopZoom();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::GamepadStopSprint()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadSprintState != EInputState::Hold) return;

	if (bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

//This function exists to cancel sprinting, when the player zooms in.
void AOPPlayer::UniversalStopSprint()
{
	if (bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::MouseAndKeyboardToggleZoom()
{
	//Make sure the input state is actually set to a "toggle".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardZoomState != EInputState::Toggle) return;
	
	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;

		//If the player is sprinting, then zooming in will cause them to stop.
		UniversalStopSprint();
	}
	else
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

void AOPPlayer::GamepadToggleZoom()
{
	//Make sure the input state is actually set to a "toggle".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadZoomState != EInputState::Toggle) return;
	
	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;

		//If the player is sprinting, then zooming in will cause them to stop.
		UniversalStopSprint();
	}
	else
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

void AOPPlayer::MouseAndKeyboardStartZoom()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardZoomState != EInputState::Hold) return;

	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;

		//If the player is sprinting, then zooming in will cause them to stop.
		UniversalStopSprint();
	}
}

void AOPPlayer::MouseAndKeyboardStopZoom()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->MouseAndKeyboardZoomState != EInputState::Hold) return;

	if (bIsPlayerZoomedIn)
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

void AOPPlayer::GamepadStartZoom()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadZoomState != EInputState::Hold) return;

	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;

		//If the player is sprinting, then zooming in will cause them to stop.
		UniversalStopSprint();
	}
}

void AOPPlayer::GamepadStopZoom()
{
	//Make sure the input state is actually set to a "hold".
	if (!IsValid(WorldSubsystem) || WorldSubsystem->GamepadZoomState != EInputState::Hold) return;

	if (bIsPlayerZoomedIn)
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

//This function exists to cancel zooming in, when the player starts sprinting.
void AOPPlayer::UniversalStopZoom()
{
	if (bIsPlayerZoomedIn)
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

void AOPPlayer::AdjustPlayerZoom(float Value)
{
	//Change the player camera's FOV, according to the current value in the zoom timeline.
	PlayerCamera->SetFieldOfView(Value);
}

void AOPPlayer::UpdateZoomCurveKeys()
{
	//This ensures that the player's zoom level will always be correct, regardless of what FOV they have set.
	if (IsValid(ZoomCurve))
	{
		ZoomCurve->FloatCurve.UpdateOrAddKey(0.f, PlayerCamera->FieldOfView);
		ZoomCurve->FloatCurve.UpdateOrAddKey(0.3f, (PlayerCamera->FieldOfView / 2.f));
	}
}

void AOPPlayer::StartFire()
{
	if (!bCanPlayerFire) return;
	if (!IsValid(CurrentWeapon) || CurrentWeapon->Stats.WeaponType == EWeaponType::NONE) return;
	if (CurrentWeapon->bFiringCooldownActive) return;

	//Play a dry-fire sound, if the current weapon is empty.
	if (CurrentWeapon->Stats.CurrentMagazine <= 0)
	{
		if (IsValid(CurrentWeapon->DryFireSound)) UGameplayStatics::PlaySoundAtLocation(this, CurrentWeapon->DryFireSound, GetActorLocation(), GetActorRotation());

		return;
	}

	FireWeapon();

	//If the current weapon is automatic, then continue firing on a looping timer.
	if (CurrentWeapon->Stats.CurrentFireMode == EFireMode::FullAuto)
	{
		GetWorldTimerManager().SetTimer(FireHandle, this, &AOPPlayer::FireWeapon, CurrentWeapon->Stats.FireRate, true);
	}
}

void AOPPlayer::FireWeapon()
{
	if (!IsValid(CurrentWeapon)) return;	

	if (IsValid(CurrentWeapon->CharacterFireMontage)) PlayAnimMontage(CurrentWeapon->CharacterFireMontage);
	
	CurrentWeapon->Shoot();

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::StopFire()
{
	//Clear the looping timer for automatic weapons.
	GetWorldTimerManager().ClearTimer(FireHandle);
}

void AOPPlayer::StartReload()
{
	if (!bCanPlayerReload) return;
	if (!IsValid(CurrentWeapon) || CurrentWeapon->Stats.WeaponType == EWeaponType::NONE) return;

	//If the player doesn't have any reserve ammo for their current weapon, then don't bother trying to reload.
	switch (CurrentWeaponType)
	{
		case EWeaponType::Pistol:
			if (PistolAmmo <= 0) return;
			break;
		case EWeaponType::Rifle:
			if (RifleAmmo <= 0) return;
			break;
		case EWeaponType::Shotgun:
			if (ShotgunAmmo <= 0) return;
			break;
		case EWeaponType::Sniper:
			if (SniperAmmo <= 0) return;
			break;
		default:
			break;
	}
	
	StopFire();
	UniversalStopZoom();

	//The player is temporarily prevented from firing, reloading, or switching weapons.
	bCanPlayerFire = false;
	bCanPlayerReload = false;
	bCanPlayerSwitch = false;

	//The weapon's reload animation plays, and a timer is set for reloading to end.
	if (IsValid(CurrentWeapon->CharacterReloadMontage))
	{
		PlayAnimMontage(CurrentWeapon->CharacterReloadMontage);
		GetWorldTimerManager().SetTimer(ReloadHandle, this, &AOPPlayer::EndReload, CurrentWeapon->CharacterReloadMontage->GetPlayLength(), false);
	}
	else
	{
		EndReload();
	}
}

void AOPPlayer::EndReload()
{
	if (!IsValid(CurrentWeapon)) return;

	//The player is once again allowed to fire, reload, and switch weapons.
	bCanPlayerFire = true;
	bCanPlayerReload = true;
	bCanPlayerSwitch = true;

	//The current weapon's category determines which type of reserve ammo will be used.
	switch (CurrentWeaponType)
	{
		case EWeaponType::Pistol:
			TakeAmmoFromReserve(PistolAmmo);
			break;
		case EWeaponType::Rifle:
			TakeAmmoFromReserve(RifleAmmo);
			break;
		case EWeaponType::Shotgun:
			TakeAmmoFromReserve(ShotgunAmmo);
			break;
		case EWeaponType::Sniper:
			TakeAmmoFromReserve(SniperAmmo);
			break;
		default:
			break;
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::TakeAmmoFromReserve(int32& ReserveAmmo)
{
	int32 AmmoUsed = CurrentWeapon->Stats.MaxMagazine - CurrentWeapon->Stats.CurrentMagazine;

	//If the player can perform a full reload, then it will be done...
	if (ReserveAmmo - AmmoUsed >= 0)
	{
		CurrentWeapon->Stats.CurrentMagazine = CurrentWeapon->Stats.MaxMagazine;
		ReserveAmmo -= AmmoUsed;
	}
	//...Otherwise, whatever reserve ammo's left will go into the magazine.
	else
	{
		CurrentWeapon->Stats.CurrentMagazine += ReserveAmmo;
		ReserveAmmo = 0;
	}
}

void AOPPlayer::CycleWeapons(const FInputActionValue& Value)
{
	if (!bCanPlayerSwitch) return;
	if (!IsValid(CurrentWeapon) || WeaponArray.Num() < 2) return;

	StopFire();
	UniversalStopZoom();

	//For when the player is unarmed.
	if (CurrentWeapon == WeaponArray[0])
	{
		//If the player is unarmed, then switching "backwards" will equip the last weapon in the array.
		if (Value.GetMagnitude() < 0)
		{
			StartSwitch(WeaponArray.Last());
		}
		else if (Value.GetMagnitude() > 0)
		{
			StartSwitch(WeaponArray[1]);
		}
	}
	//For when the player is holding their first weapon.
	else if (CurrentWeapon == WeaponArray[1])
	{
		//If the player is holding their first weapon, then switching "backwards" will equip the last weapon in the array.
		if (Value.GetMagnitude() < 0)
		{
			StartSwitch(WeaponArray.Last());
		}
		else if (Value.GetMagnitude() > 0)
		{
			if (WeaponArray.Num() >= 3)
			{
				StartSwitch(WeaponArray[2]);
			}
			else
			{
				StartSwitch(CurrentWeapon);
			}
		}
	}
	//For when the player is holding their second weapon, and on.
	else
	{
		if (Value.GetMagnitude() < 0)
		{
			StartSwitch(WeaponArray[WeaponArray.Find(CurrentWeapon) - 1]);
		}
		else if (Value.GetMagnitude() > 0)
		{
			//If the player is holding the last weapon in the array, then switching "forwards" will equip their first weapon.
			if (CurrentWeapon == WeaponArray.Last())
			{
				StartSwitch(WeaponArray[1]);
			}
			else
			{
				StartSwitch(WeaponArray[WeaponArray.Find(CurrentWeapon) + 1]);
			}
		}
	}
}

void AOPPlayer::StartSwitch(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;

	//The player is temporarily prevented from firing, reloading, or switching weapons.
	bCanPlayerFire = false;
	bCanPlayerReload = false;
	bCanPlayerSwitch = false;

	//The weapon's unequip animation plays, and a timer is set for switching to end.
	if (IsValid(CurrentWeapon->CharacterUnequipMontage))
	{
		PlayAnimMontage(CurrentWeapon->CharacterUnequipMontage);

		//An FTimerDelegate is needed, to call a function with parameters on a timer.
		FTimerDelegate SwitchDelegate;
		SwitchDelegate.BindUFunction(this, TEXT("EndSwitch"), NewWeapon);
		
		GetWorldTimerManager().SetTimer(SwitchHandle, SwitchDelegate, CurrentWeapon->CharacterUnequipMontage->GetPlayLength(), false);
	}
	else
	{
		EndSwitch(NewWeapon);
	}
	
}

void AOPPlayer::EndSwitch(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;

	HideAllUnequippedWeapons(NewWeapon);

	//The new weapon's equip animation plays, and a timer is set for the player to capable of firing, reloading, and switching weapons again.
	if (IsValid(CurrentWeapon->CharacterEquipMontage))
	{
		PlayAnimMontage(CurrentWeapon->CharacterEquipMontage);
		
		GetWorldTimerManager().SetTimer(SwitchHandle, this, &AOPPlayer::AddPlayerTagsAfterWeaponSwitch, CurrentWeapon->CharacterEquipMontage->GetPlayLength(), false);
	}
	else
	{
		AddPlayerTagsAfterWeaponSwitch();
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::HideAllUnequippedWeapons(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;

	for (TObjectPtr<AOPWeapon> Index : WeaponArray)
	{
		//If the weapon references don't match, then hide the weapon...
		if (Index != NewWeapon)
		{
			Index->SetActorHiddenInGame(true);
		}
		//...But if they DO match, then show the weapon and designate it as the current weapon.
		else
		{
			Index->SetActorHiddenInGame(false);

			CurrentWeapon = Index;
			CurrentWeaponType = Index->Stats.WeaponType;
		}
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::AddPlayerTagsAfterWeaponSwitch()
{
	//The player is once again allowed to fire, reload, and switch weapons.
	bCanPlayerFire = true;
	bCanPlayerReload = true;
	bCanPlayerSwitch = true;

	//Clear the time for weapon switching.
	GetWorldTimerManager().ClearTimer(SwitchHandle);
}

void AOPPlayer::ChangeFireMode()
{
	if (!bCanPlayerFire) return;
	if (!IsValid(CurrentWeapon) || CurrentWeapon->Stats.WeaponType == EWeaponType::NONE) return;
	if (CurrentWeapon->bFiringCooldownActive) return;

	StopFire();

	//The current weapon's fire mode will change, based on which ones are available.
	switch (CurrentWeapon->Stats.CurrentFireMode)
	{
		case EFireMode::SemiAuto:
			if (CurrentWeapon->Stats.bDoesWeaponSupportBurst)
			{
				CurrentWeapon->Stats.CurrentFireMode = EFireMode::Burst;
			}
			else if (!CurrentWeapon->Stats.bDoesWeaponSupportBurst && CurrentWeapon->Stats.bDoesWeaponSupportAuto)
			{
				CurrentWeapon->Stats.CurrentFireMode = EFireMode::FullAuto;
			}
			break;
		case EFireMode::Burst:
			if (CurrentWeapon->Stats.bDoesWeaponSupportAuto)
			{
				CurrentWeapon->Stats.CurrentFireMode = EFireMode::FullAuto;
			}
			else
			{
				CurrentWeapon->Stats.CurrentFireMode = EFireMode::SemiAuto;
			}
			break;
		case EFireMode::FullAuto:
			CurrentWeapon->Stats.CurrentFireMode = EFireMode::SemiAuto;
			break;
		default:
			break;
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::Interact()
{
	if (!bCanPlayerInteract) return;

	//Double-check the last focused actor for an interface, before trying to interact with it.
	if (IsValid(FocusedActor) && FocusedActor->Implements<UOPInteractInterface>())
	{
		IOPInteractInterface::Execute_OnInteract(FocusedActor, this);
		bCanPlayerInteract = false;

		//Clear the interact prompt in the player's HUD.
		OnInteractUpdate.Broadcast(FText(), EInteractType::NONE);
	}
}

void AOPPlayer::InteractLineTrace()
{
	if (IsValid(GetController()))
	{
		//Store the player camera's location and rotation in a pair of out parameters.
		GetController()->GetPlayerViewPoint(CameraLocation, CameraRotation);

		//The player's interact radius determines where the line trace will end.
		FVector EndLocation = CameraLocation + CameraRotation.Vector() * InteractRadius;

		//Interact line traces should always ignore the player themselves.
		TArray<TObjectPtr<AActor>> ActorsToIgnore;
		ActorsToIgnore.Emplace(this);
		
		//Show debug lines for the line trace, if they've been globally enabled.
		if (IsValid(WorldSubsystem) && WorldSubsystem->bInteractDebugLinesEnabled)
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, InteractHitResult, true, FLinearColor::Red, FLinearColor::Green, 2.f);
		}
		else
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, InteractHitResult, true, FLinearColor::Red, FLinearColor::Green, 0.f);
		}

		CheckForInteractableObjects();
	}
}

void AOPPlayer::CheckForInteractableObjects()
{
	TObjectPtr<AActor> HitActor = InteractHitResult.GetActor();

	if (IsValid(HitActor))
	{
		//If the hit actor is the same as the last focused actor, then no interface check is needed.
		if (HitActor != FocusedActor)
		{
			//Check the last focused actor for an interface, and end focus if possible.
			if (IsValid(FocusedActor) && FocusedActor->Implements<UOPInteractInterface>())
			{
				IOPInteractInterface::Execute_EndFocus(FocusedActor);
				bCanPlayerInteract = false;

				//Clear the interact prompt in the player's HUD.
				OnInteractUpdate.Broadcast(FText(), EInteractType::NONE);
			}

			//Check the hit actor for an interface, and start focus if possible.
			if (IsValid(HitActor) && HitActor->Implements<UOPInteractInterface>())
			{
				IOPInteractInterface::Execute_StartFocus(HitActor);
				bCanPlayerInteract = true;

				//Update the interact prompt in the player's HUD, with information about the focused object.
				OnInteractUpdate.Broadcast(IOPInteractInterface::Execute_GetInteractableObjectName(HitActor), IOPInteractInterface::Execute_GetInteractableObjectType(HitActor));
			}
		}

		//Regardless of what happens, a reference to the hit actor is stored.
		FocusedActor = HitActor;
	}
	//For when no actors were hit by the line trace.
	else
	{
		//Check the last focused actor for an interface, and end focus if possible.
		if (IsValid(FocusedActor) && FocusedActor->Implements<UOPInteractInterface>())
		{
			IOPInteractInterface::Execute_EndFocus(FocusedActor);
			bCanPlayerInteract = false;

			//Clear the interact prompt in the player's HUD.
			OnInteractUpdate.Broadcast(FText(), EInteractType::NONE);
		}

		//Since no actor was hit, no reference needs to be stored.
		FocusedActor = nullptr;
	}
}

void AOPPlayer::StartMelee()
{
	
}

void AOPPlayer::StopMelee()
{
	
}

void AOPPlayer::CharacterDeath()
{
	if (bIsCharacterDead) return;

	Super::CharacterDeath();

	//LOGIC FOR MAKING THE PLAYER DROP THEIR CURRENT WEAPON GOES HERE
	//LOGIC FOR STARTING THE "GAME OVER" SEQUENCE GOES HERE
}

void AOPPlayer::ReplenishReserveAmmo(EWeaponType AmmoType)
{
	//If infinite ammo with reloading is enabled, then the player will get reserve ammo every time they fire.
	switch(AmmoType)
	{
		case EWeaponType::Pistol:
			PistolAmmo = FMath::Clamp((PistolAmmo + 1), 0, MaxReserveAmmo);
			break;
		case EWeaponType::Rifle:
			RifleAmmo = FMath::Clamp((RifleAmmo + 1), 0, MaxReserveAmmo);
			break;
		case EWeaponType::Shotgun:
			ShotgunAmmo = FMath::Clamp((ShotgunAmmo + 1), 0, MaxReserveAmmo);
			break;
		case EWeaponType::Sniper:
			SniperAmmo = FMath::Clamp((SniperAmmo + 1), 0, MaxReserveAmmo);
			break;
		default:
			break;
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	//The character cannot have a health value below 0.
	CurrentHealth = FMath::Clamp((CurrentHealth - Damage), 0, MaxHealth);

	//If the character has run out of health, then they die.
	if (CurrentHealth <= 0)
	{
		CharacterDeath();
	}
}

void AOPPlayer::MeleeSphereTrace_Implementation(FVector MeleeStart, FVector MeleeEnd, float Radius)
{
	//The player should never be hit by their own melee attack.
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	ActorsToIgnore.Emplace(this);

	if (IsValid(WorldSubsystem) && WorldSubsystem->bMeleeDebugLinesEnabled)
	{
		UKismetSystemLibrary::SphereTraceMulti(this, MeleeStart, MeleeEnd, Radius, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, MeleeHitResults, true, FLinearColor::Red, FLinearColor::Green, 2.5f);
	}
	else
	{
		UKismetSystemLibrary::SphereTraceMulti(this, MeleeStart, MeleeEnd, Radius, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, MeleeHitResults, true, FLinearColor::Red, FLinearColor::Green, 0.f);
	}

	ProcessMeleeHitOnTargets_Implementation();
}

void AOPPlayer::PickUpWeapon_Implementation(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;
	
	for (TObjectPtr<AOPWeapon> Index : WeaponArray)
	{
		//If the player picks up more than one of the same weapon, then give them reserve ammo instead.
		if (Index->Stats.WeaponName.EqualTo(NewWeapon->Stats.WeaponName))
		{
			if (!IsPlayerReserveAmmoMaxedOut_Implementation(Index->Stats.WeaponType))
			{
				PickUpAmmo_Implementation(Index->Stats.WeaponType, FMath::RandRange(Index->Stats.MaxMagazine / 2, Index->Stats.MaxMagazine));
				NewWeapon->Destroy();
			}

			return;
		}
	}

	FAttachmentTransformRules StartingRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	/*
	The player is set as the weapon's owner, it gets attached to their mesh...
	...The weapon's mesh no longer collides with pawns, or casts shadows...
	...And finally, it is added to the player's weapon array.
	*/
	NewWeapon->SetOwner(this);
	NewWeapon->AttachToComponent(GetMesh(), StartingRules, NewWeapon->AttachToSocket);
	NewWeapon->WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponArray.Emplace(NewWeapon);

	//Sort the weapon array by category, in ascending order.
	WeaponArray.Sort([](const AOPWeapon& a, const AOPWeapon& b) {return a.Stats.WeaponType < b.Stats.WeaponType;});

	//If this is the first weapon that the player has picked up, then it will be automatically equipped.
	if (WeaponArray.Num() <= 2)
	{
		HideAllUnequippedWeapons(NewWeapon);
	}
	else
	{
		HideAllUnequippedWeapons(CurrentWeapon);
	}

	//Broadcast info about the item that was picked up, to the player's HUD.
	OnItemPickup.Broadcast(true, false, false);
}

void AOPPlayer::PickUpAmmo_Implementation(EWeaponType AmmoType, int32 Amount)
{
	if (IsPlayerReserveAmmoMaxedOut_Implementation(AmmoType)) return;
	
	/*
	The player receives more reserve ammo, based on the type that they picked up.
	The amount they receive should never put them over their reserve ammo limit.
	*/
	switch (AmmoType)
	{
		case EWeaponType::Pistol:
			PistolAmmo = FMath::Clamp(PistolAmmo + Amount, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Rifle:
			RifleAmmo = FMath::Clamp(RifleAmmo + Amount, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Shotgun:
			ShotgunAmmo = FMath::Clamp(ShotgunAmmo + Amount, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Sniper:
			SniperAmmo = FMath::Clamp(SniperAmmo + Amount, 0, MaxReserveAmmo);
			break;
		default:
			break;
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();

	//Broadcast info about the item that was picked up, to the player's HUD.
	OnItemPickup.Broadcast(false, true, false);
}

bool AOPPlayer::IsPlayerReserveAmmoMaxedOut_Implementation(EWeaponType TypeToCheck)
{
	//Checks if the player can no longer carry any of the reserve ammo in question.
	switch (TypeToCheck)
	{
	case EWeaponType::Pistol:
		return (PistolAmmo == MaxReserveAmmo);
	case EWeaponType::Rifle:
		return (RifleAmmo == MaxReserveAmmo);
	case EWeaponType::Shotgun:
		return (ShotgunAmmo == MaxReserveAmmo);
	case EWeaponType::Sniper:
		return (SniperAmmo == MaxReserveAmmo);
	default:
		break;
	}

	return false;
}