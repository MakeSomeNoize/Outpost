// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

	//Bind a callback function to the zoom timeline.
	if (IsValid(ZoomCurve))
	{
		ZoomFunction.BindUFunction(this, TEXT("AdjustPlayerZoom"));
		ZoomTimeline->AddInterpFloat(ZoomCurve, ZoomFunction);
	}
}

// Called every frame
void AOPPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	
}

void AOPPlayer::MoveBackward(const FInputActionValue& Value)
{
	
}

void AOPPlayer::StrafeLeft(const FInputActionValue& Value)
{
	
}

void AOPPlayer::StrafeRight(const FInputActionValue& Value)
{
	
}

void AOPPlayer::GamepadMove(const FInputActionValue& Value)
{
	
}

void AOPPlayer::MouseLook(const FInputActionValue& Value)
{
	
}

void AOPPlayer::GamepadLook(const FInputActionValue& Value)
{
	
}

void AOPPlayer::ToggleZoom()
{
	
}

void AOPPlayer::StartZoom()
{
	
}

void AOPPlayer::StopZoom()
{
	
}

void AOPPlayer::AdjustPlayerZoom(float Value)
{
	//Change the player camera's FOV, according to the current value in the zoom timeline.
	PlayerCamera->SetFieldOfView(Value);
}

void AOPPlayer::ToggleCrouch()
{
	if (!bIsCrouched)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AOPPlayer::StartCrouch()
{
	if (!bIsCrouched) Crouch();
}

void AOPPlayer::StopCrouch()
{
	if (bIsCrouched) UnCrouch();
}

void AOPPlayer::ToggleSprint()
{
	
}

void AOPPlayer::StartSprint()
{
	
}

void AOPPlayer::StopSprint()
{
	
}

void AOPPlayer::StartFire()
{
	
}

void AOPPlayer::FireWeapon()
{
	
}

void AOPPlayer::StopFire()
{
	
}

void AOPPlayer::StartReload()
{
	
}

void AOPPlayer::EndReload()
{
	
}

void AOPPlayer::TakeAmmoFromReserve(int32& AmmoToTake)
{
	
}

void AOPPlayer::CycleWeapons(const FInputActionValue& Value)
{
	
}

void AOPPlayer::StartSwitch()
{
	
}

void AOPPlayer::EndSwitch()
{
	
}

void AOPPlayer::Interact()
{
	
}

void AOPPlayer::InteractLineTrace()
{
	
}

void AOPPlayer::CheckForInteractableObjects()
{
	
}

void AOPPlayer::StartMelee()
{
	
}

void AOPPlayer::MeleeSphereTrace()
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
		if (IsValid(WorldSubsystem))
		{
			//Change the "zoom" input behavior from a toggle to a hold, if the player so chooses.
			if (WorldSubsystem->bHoldZoomEnabled && IsValid(HoldZoomAction))
			{
				PlayerEnhancedInputComponent->BindAction(HoldZoomAction, ETriggerEvent::Started, this, &AOPPlayer::StartZoom);
				PlayerEnhancedInputComponent->BindAction(HoldZoomAction, ETriggerEvent::Completed, this, &AOPPlayer::StopZoom);
			}

			//Change the "crouch" input behavior from a toggle to a hold, if the player so chooses.
			if (WorldSubsystem->bHoldCrouchEnabled && IsValid(HoldCrouchAction))
			{
				PlayerEnhancedInputComponent->BindAction(HoldCrouchAction, ETriggerEvent::Started, this, &AOPPlayer::StartCrouch);
				PlayerEnhancedInputComponent->BindAction(HoldCrouchAction, ETriggerEvent::Completed, this, &AOPPlayer::StopCrouch);
			}

			//Change the "sprint" input behavior from a toggle to a hold, if the player so chooses.
			if (WorldSubsystem->bHoldSprintEnabled && IsValid(HoldSprintAction))
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
}