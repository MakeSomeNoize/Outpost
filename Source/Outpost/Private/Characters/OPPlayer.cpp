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

	//LOGIC FOR ADDING STARTING WEAPONS GOES HERE
	
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

void AOPPlayer::ToggleZoom()
{
	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;
	}
	else
	{
		ZoomTimeline->Reverse();
		bIsPlayerZoomedIn = false;
	}
}

void AOPPlayer::StartZoom()
{
	if (!bIsPlayerZoomedIn)
	{
		ZoomTimeline->Play();
		bIsPlayerZoomedIn = true;
	}
}

void AOPPlayer::StopZoom()
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

void AOPPlayer::ToggleCrouch()
{
	if (!bIsPlayerCrouching)
	{
		Crouch();
		bIsPlayerCrouching = true;

		//If the player is sprinting, crouching will cause them to stop.
		StopSprint();
	}
	else
	{
		UnCrouch();
		bIsPlayerCrouching = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartCrouch()
{
	if (!bIsPlayerCrouching)
	{
		Crouch();
		bIsPlayerCrouching = true;

		//If the player is sprinting, crouching will cause them to stop.
		StopSprint();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StopCrouch()
{
	if (bIsPlayerCrouching)
	{
		UnCrouch();
		bIsPlayerCrouching = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::ToggleSprint()
{
	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is crouching, sprinting will cause them to stand up.
		StopCrouch();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartSprint()
{
	if (!bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsPlayerSprinting = true;

		//If the player is crouching, sprinting will cause them to stand up.
		StopCrouch();
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StopSprint()
{
	if (bIsPlayerSprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed;
		bIsPlayerSprinting = false;
	}

	//Update the movement status in the player's HUD.
	OnMovementUpdate.Broadcast();
}

void AOPPlayer::StartFire()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPPlayer::FireWeapon()
{
	
}

void AOPPlayer::StopFire()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPPlayer::StartReload()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPPlayer::EndReload()
{
	
}

void AOPPlayer::TakeAmmoFromReserve(int32& AmmoToTake)
{
	
}

void AOPPlayer::CycleWeapons(const FInputActionValue& Value)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPPlayer::StartSwitch()
{
	
}

void AOPPlayer::EndSwitch()
{
	
}

void AOPPlayer::Interact()
{
	if (!bCanPlayerInteract) return;

	//Double-check the last focused actor for an interface, before trying to interact with it.
	if (IsValid(FocusedActor) && FocusedActor->Implements<UOPInteractInterface>())
	{
		IOPInteractInterface::Execute_OnInteract(FocusedActor, this);
		bCanPlayerInteract = false;
	}

	//Update the interact prompt in the player's HUD.
	OnInteractUpdate.Broadcast(FocusedItemText);

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
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
		if (IsValid(WorldSubsystem) && WorldSubsystem->bDebugLinesEnabled)
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
			}

			//Check the hit actor for an interface, and start focus if possible.
			if (IsValid(HitActor) && HitActor->Implements<UOPInteractInterface>())
			{
				IOPInteractInterface::Execute_StartFocus(HitActor);
				bCanPlayerInteract = true;

				FocusedItemText = IOPInteractInterface::Execute_GetInteractMessage(HitActor);
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

			FocusedItemText = FText();
		}

		//Since no actor was hit, no reference needs to be stored.
		FocusedActor = nullptr;
	}

	//Update the interact prompt in the player's HUD.
	OnInteractUpdate.Broadcast(FocusedItemText);
}

void AOPPlayer::StartMelee()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
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