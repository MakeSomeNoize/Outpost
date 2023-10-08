// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Items/OPWeapon.h"
#include "Interfaces/OPInteractInterface.h"

// Sets default values
AOPPlayer::AOPPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	PlayerCamera->SetupAttachment(RootComponent);
	PlayerCamera->bUsePawnControlRotation = true;

	//Enables crouching for the player.
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

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
		StartingWeapon->AttachToComponent(GetMesh(), StartingRules, TEXT("FPS_Weapon_L"));
		StartingWeapon->WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		StartingWeapon->WeaponMesh->SetCastShadow(false);
		WeaponArray.Emplace(StartingWeapon);
		CurrentWeapon = StartingWeapon;
		CurrentWeaponType = CurrentWeapon->WeaponType;
	}
	else
	{
		CurrentWeapon = WeaponArray[0];
	}

	//Bind a callback function to OnInfiniteAmmo delegate, if applicable.
	if (IsValid(WorldSubsystem) && WorldSubsystem->bInfiniteAmmoWithReloadEnabled) WorldSubsystem->OnInfiniteAmmoWithReloadUpdate.AddDynamic(this, &AOPPlayer::DebugReplenishReserveAmmo);
	
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

		if (IsValid(JumpAction)) PlayerEnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AOPPlayer::StartJump);

		if (IsValid(MouseLookAction)) PlayerEnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AOPPlayer::MouseLook);
		if (IsValid(GamepadLookAction)) PlayerEnhancedInputComponent->BindAction(GamepadLookAction, ETriggerEvent::Triggered, this, &AOPPlayer::GamepadLook);

		if (IsValid(ToggleZoomAction)) PlayerEnhancedInputComponent->BindAction(ToggleZoomAction, ETriggerEvent::Triggered, this, &AOPPlayer::ToggleZoom);
		
		if (IsValid(ToggleCrouchAction)) PlayerEnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Triggered, this, &AOPPlayer::ToggleCrouch);

		if (IsValid(ToggleSprintAction)) PlayerEnhancedInputComponent->BindAction(ToggleSprintAction, ETriggerEvent::Triggered, this, &AOPPlayer::ToggleSprint);

		if (IsValid(FireWeaponAction))
		{
			PlayerEnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &AOPPlayer::StartFire);
			PlayerEnhancedInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &AOPPlayer::StopFire);
		}

		if (IsValid(ReloadWeaponAction)) PlayerEnhancedInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Triggered, this, &AOPPlayer::StartReload);

		if (IsValid(CycleWeaponsAction)) PlayerEnhancedInputComponent->BindAction(CycleWeaponsAction, ETriggerEvent::Triggered, this, &AOPPlayer::CycleWeapons);
		
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
			if (IsValid(BaseContext)) Subsystem->AddMappingContext(BaseContext, 0);
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

void AOPPlayer::StartJump()
{
	//If the player is currently crouching, then jumping will cause them to stop.
	if (CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching")))
	{
		StopCrouch();
	}
	else
	{
		Jump();
	}
}

void AOPPlayer::ToggleZoom()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn")))
	{
		ZoomTimeline->Play();
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn"));

		//If the player is sprinting, then zooming in will cause them to stop.
		StopSprint();
	}
	else
	{
		ZoomTimeline->Reverse();
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn"));
	}
}

void AOPPlayer::StartZoom()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn")))
	{
		ZoomTimeline->Play();
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn"));

		//If the player is sprinting, then zooming in will cause them to stop.
		StopSprint();
	}
}

void AOPPlayer::StopZoom()
{
	if (CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn")))
	{
		ZoomTimeline->Reverse();
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsZoomedIn"));
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

void AOPPlayer::ToggleCrouch()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching")))
	{
		Crouch();
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching"));

		//If the player is sprinting, then crouching will cause them to stop.
		StopSprint();
	}
	else
	{
		UnCrouch();
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching"));
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartCrouch()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching")))
	{
		Crouch();
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching"));

		//If the player is sprinting, then crouching will cause them to stop.
		StopSprint();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StopCrouch()
{
	if (CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching")))
	{
		UnCrouch();
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsCrouching"));
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::ToggleSprint()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting")))
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting"));

		//If the player is zoomed in and/or crouching, then sprinting will cause them to stop.
		StopZoom();
		StopCrouch();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting"));
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartSprint()
{
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting")))
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting"));

		//If the player is zoomed in and/or crouching, then sprinting will cause them to stop.
		StopZoom();
		StopCrouch();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StopSprint()
{
	if (CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting")))
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.IsSprinting"));
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartFire()
{
	if (!IsValid(CurrentWeapon) || CurrentWeapon->WeaponType == EWeaponType::NONE) return;
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.CanFire"))) return;
	if (CurrentWeapon->WeaponTags.HasTagExact(FGameplayTag::RequestGameplayTag("Weapon.FiringCooldownActive"))) return;
	if (CurrentWeapon->WeaponTags.HasTagExact(FGameplayTag::RequestGameplayTag("Weapon.AnimationCooldownActive"))) return;

	if (CurrentWeapon->CurrentMagazine <= 0)
	{
		//PLAY DRY-FIRE MONTAGE HERE

		return;
	}

	FireWeapon();

	//If the current weapon is automatic, then continue firing on a looping timer.
	if (CurrentWeapon->WeaponTags.HasTagExact(FGameplayTag::RequestGameplayTag("Weapon.IsAutomatic")))
	{
		GetWorldTimerManager().SetTimer(FireHandle, this, &AOPPlayer::FireWeapon, CurrentWeapon->FireRate, true);
	}
}

void AOPPlayer::FireWeapon()
{
	if (!IsValid(CurrentWeapon)) return;	

	//PLAY THE CURRENT WEAPON'S FIRING MONTAGE HERE
	
	CurrentWeapon->Shoot();

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::StopFire()
{
	GetWorldTimerManager().ClearTimer(FireHandle);
	
}

void AOPPlayer::StartReload()
{
	if (!IsValid(CurrentWeapon) || CurrentWeapon->WeaponType == EWeaponType::NONE) return;
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.CanReload"))) return;

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
	StopZoom();

	//The player is temporarily prevented from firing, reloading, or switching weapons.
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanFire"));
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanReload"));
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanSwitch"));

	EndReload(); //PLACEHOLDER

	//PLAYING OF RELOAD MONTAGE GOES HERE
	//STARTING OF TIMER FOR ENDRELOAD() GOES HERE
}

void AOPPlayer::EndReload()
{
	if (!IsValid(CurrentWeapon)) return;

	//The player is once again allowed to fire, reload, and switch weapons.
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanFire"));
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanReload"));
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanSwitch"));

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
	int32 AmmoUsed = CurrentWeapon->MaxMagazine - CurrentWeapon->CurrentMagazine;

	//If the player can perform a full reload, then it will be done...
	if (ReserveAmmo - AmmoUsed >= 0)
	{
		CurrentWeapon->CurrentMagazine = CurrentWeapon->MaxMagazine;
		ReserveAmmo -= AmmoUsed;
	}
	//...Otherwise, whatever reserve ammo's left will go into the magazine.
	else
	{
		CurrentWeapon->CurrentMagazine += ReserveAmmo;
		ReserveAmmo = 0;
	}
}

void AOPPlayer::CycleWeapons(const FInputActionValue& Value)
{
	//The player can't switch weapons if they only have one.
	if (!IsValid(CurrentWeapon) && WeaponArray.Num() < 2) return;
	if (!CharacterTags.HasTagExact(FGameplayTag::RequestGameplayTag("Character.Player.CanSwitch"))) return;

	StopFire();
	StopZoom();

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
		//If the player is holding their first weapon, then switching "forwards" will equip the last weapon in the array.
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
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanFire"));
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanReload"));
	CharacterTags.RemoveTag(FGameplayTag::RequestGameplayTag("Character.Player.CanSwitch"));

	//PLAY UNEQUIP MONTAGE FOR CURRENT WEAPON HERE

	//An FTimerDelegate is needed, to call a function with parameters on a timer.
	FTimerDelegate SwitchDelegate;
	SwitchDelegate.BindUFunction(this, TEXT("EndSwitch"), NewWeapon);

	//Set a timer for when weapon switching will end.
	GetWorldTimerManager().SetTimer(SwitchHandle, SwitchDelegate, 0.1f, false); //INRATE IS A PLACEHOLDER VALUE
}

void AOPPlayer::EndSwitch(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;

	HideAllUnequippedWeapons(NewWeapon);

	//PLAY EQUIP MONTAGE FOR NEW WEAPON HERE

	//Set a timer for when player tags will be re-added.
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AOPPlayer::AddPlayerTagsAfterWeaponSwitch, 0.1f, false); //INRATE IS A PLACEHOLDER VALUE

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
			CurrentWeaponType = Index->WeaponType;
		}
	}

	//FOR TESTING ONLY
	for (TObjectPtr<AOPWeapon> Index : WeaponArray)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("Index %u: %s"), WeaponArray.Find(Index), *(Index->WeaponName.ToString()))); //FOR TESTING ONLY
	}

	//Update the weapon info in the player's HUD.
	OnWeaponUpdate.Broadcast();
}

void AOPPlayer::AddPlayerTagsAfterWeaponSwitch()
{
	//The player is once again allowed to fire, reload, and switch weapons.
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanFire"));
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanReload"));
	CharacterTags.AddTag(FGameplayTag::RequestGameplayTag("Character.Player.CanSwitch"));

	GetWorldTimerManager().ClearTimer(SwitchHandle);
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
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, InteractHitResult, true, FLinearColor::Red, FLinearColor::Green, 2.5f);
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

void AOPPlayer::BindExtraInputBehaviorToPlayer()
{
	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		//Change the "zoom" input behavior from a toggle to a hold, if the player so chooses.
		if (bHoldZoomEnabled && IsValid(HoldZoomAction))
		{
			PlayerEnhancedInputComponent->BindAction(HoldZoomAction, ETriggerEvent::Started, this, &AOPPlayer::StartZoom);
			PlayerEnhancedInputComponent->BindAction(HoldZoomAction, ETriggerEvent::Completed, this, &AOPPlayer::StopZoom);
		}

		//Change the "crouch" input behavior from a toggle to a hold, if the player so chooses.
		if (bHoldCrouchEnabled && IsValid(HoldCrouchAction))
		{
			PlayerEnhancedInputComponent->BindAction(HoldCrouchAction, ETriggerEvent::Started, this, &AOPPlayer::StartCrouch);
			PlayerEnhancedInputComponent->BindAction(HoldCrouchAction, ETriggerEvent::Completed, this, &AOPPlayer::StopCrouch);
		}

		//Change the "sprint" input behavior from a toggle to a hold, if the player so chooses.
		if (bHoldSprintEnabled && IsValid(HoldSprintAction))
		{
			PlayerEnhancedInputComponent->BindAction(HoldSprintAction, ETriggerEvent::Started, this, &AOPPlayer::StartSprint);
			PlayerEnhancedInputComponent->BindAction(HoldSprintAction, ETriggerEvent::Completed, this, &AOPPlayer::StopSprint);
		}

		// Make sure that we have a valid PlayerController.
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				//Add the IMC that contains the "hold" inputs that the player wants to use.
				if (IsValid(ExtraContext)) Subsystem->AddMappingContext(ExtraContext, 1);
			}
		}
	}
}

void AOPPlayer::CharacterDeath()
{
	Super::CharacterDeath();
	
}

void AOPPlayer::DebugReplenishReserveAmmo(EWeaponType AmmoType)
{
	//If infinite ammo with reloading is enabled, then the player will get reserve ammo every time they fire.
	switch(AmmoType)
	{
		case EWeaponType::Pistol:
			PistolAmmo = FMath::Clamp(PistolAmmo++, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Rifle:
			RifleAmmo = FMath::Clamp(RifleAmmo++, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Shotgun:
			ShotgunAmmo = FMath::Clamp(ShotgunAmmo++, 0, MaxReserveAmmo);
			break;
		case EWeaponType::Sniper:
			SniperAmmo = FMath::Clamp(SniperAmmo++, 0, MaxReserveAmmo);
			break;
		default:
			break;
	}
}

void AOPPlayer::PickUpWeapon_Implementation(AOPWeapon* NewWeapon)
{
	if (!IsValid(NewWeapon)) return;

	//The player cannot pick up more than one of the same weapon.
	for (TObjectPtr<AOPWeapon> Index : WeaponArray)
	{
		if (Index->WeaponName.EqualTo(NewWeapon->WeaponName)) return;
	}

	FAttachmentTransformRules StartingRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	/*
	The player is set as the weapon's owner, it gets attached to their mesh...
	...The weapon's mesh no longer collides with pawns, or casts shadows...
	...And finally, it is added to the player's weapon array.
	*/
	NewWeapon->SetOwner(this);
	NewWeapon->AttachToComponent(GetMesh(), StartingRules, TEXT("FPS_Weapon_L"));
	NewWeapon->WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponArray.Emplace(NewWeapon);

	//Sort the weapon array by category, in ascending order.
	WeaponArray.Sort([](const AOPWeapon& a, const AOPWeapon& b) {return a.WeaponType < b.WeaponType;});

	//If this is the first weapon that the player has picked up, then it will be automatically equipped.
	if (WeaponArray.Num() <= 2)
	{
		HideAllUnequippedWeapons(NewWeapon);
	}
	else
	{
		HideAllUnequippedWeapons(CurrentWeapon);
	}

	//LOGIC FOR BROADCASTING TO "PICK UP ITEM" DELEGATE GOES HERE
}