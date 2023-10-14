// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/OPWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/OPWorldSubsystem.h"
#include "Interfaces/OPCharacterInterface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AOPWeapon::AOPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>("Weapon Root");
	RootComponent = WeaponRoot;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon Mesh");
	WeaponMesh->SetupAttachment(WeaponRoot);
	WeaponMesh->SetGenerateOverlapEvents(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
}

// Called when the game starts or when spawned
void AOPWeapon::BeginPlay()
{
	Super::BeginPlay();

	//Get a reference to the world subsystem.
	WorldSubsystem = GetWorld()->GetSubsystem<UOPWorldSubsystem>();

	Stats.CurrentMagazine = Stats.MaxMagazine;

	//Shotguns are not allowed to be automatic or burst-fire.
	if (Stats.WeaponType == EWeaponType::Shotgun)
	{
		Stats.DefaultFireMode = EFireMode::SemiAuto;
		Stats.bDoesWeaponSupportAuto = false;
		Stats.bDoesWeaponSupportBurst = false;
	}

	Stats.CurrentFireMode = Stats.DefaultFireMode;
	
}

// Called every frame
void AOPWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOPWeapon::Shoot()
{
	//The weapon cannot shoot, if its magazine is empty.
	if (Stats.CurrentMagazine <= 0) return;

	//If the weapon is a shotgun, then all of its shots will fire at once.
	if (Stats.WeaponType == EWeaponType::Shotgun)
	{
		for (int i = 0; i < Stats.ShotAmount; i++)
		{
			WeaponLineTrace();
		}
	}
	
	//If the weapon is burst-fire, then multiple shots will fire in sequence...
	if (Stats.CurrentFireMode == EFireMode::Burst && BurstCount < Stats.ShotAmount)
	{
		if (IsValid(WeaponShootMontage)) WeaponMesh->PlayAnimation(WeaponShootMontage, false);

		//BURST-FIRE LOGIC GOES HERE
	}
	//...Otherwise, only one shot will be fired.
	else
	{
		if (IsValid(WeaponShootMontage)) WeaponMesh->PlayAnimation(WeaponShootMontage, false);

		WeaponLineTrace();
	}

	//If the weapon is semi-automatic, then start a cooldown and set a timer for when the cooldown will end.
	if (Stats.CurrentFireMode == EFireMode::SemiAuto)
	{
		bFiringCooldownActive = true;

		GetWorldTimerManager().SetTimer(FiringCooldownHandle, this, &AOPWeapon::EndFiringCooldown, Stats.FireRate, false);
	}
	
	if (IsValid(WorldSubsystem)) CheckInfiniteAmmoStatus();
}

void AOPWeapon::WeaponLineTrace()
{
	//Force-initializes the weapon hit result, so that it is unique each time.
	WeaponHitResult = FHitResult(ForceInit);

	//Weapon line traces should always ignore the weapon itself, as well as its owner.
	TArray<TObjectPtr<AActor>> ActorsToIgnore;
	ActorsToIgnore.Emplace(this);
	ActorsToIgnore.Emplace(GetOwner());

	//Need to get a reference to the controller of the weapon's owner.
	TObjectPtr<AController> Controller = GetOwner()->GetInstigatorController();
	
	if (IsValid(Controller))
	{
		//Store the player camera's location and rotation in a pair of out parameters.
		Controller->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector EndLocation = CalculateWeaponSpread();

		//Show debug lines for the line trace, if they've been globally enabled.
		if (IsValid(WorldSubsystem) && WorldSubsystem->bWeaponDebugLinesEnabled)
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, WeaponHitResult, true, FLinearColor::Red, FLinearColor::Green, 2.5f);
		}
		else
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, WeaponHitResult, true, FLinearColor::Red, FLinearColor::Green, 0.f);
		}

		ApplyDamageAndParticleEffectToTarget();
	}
}

FVector AOPWeapon::CalculateWeaponSpread()
{
	//The angle of the shot is randomly generated within a cone-shaped area.
	FVector ShotAngle = FMath::VRandCone(CameraRotation.Vector(), Stats.SpreadRadius, Stats.SpreadRadius);

	return CameraLocation + ShotAngle * Stats.MaxRange;
}

void AOPWeapon::ApplyDamageAndParticleEffectToTarget()
{
	TObjectPtr<AActor> Target = WeaponHitResult.GetActor();
	
	if (IsValid(Target))
	{
		//Calculate the direction that the shot came from.
		FVector ShotFromDirection = (WeaponHitResult.TraceEnd - WeaponHitResult.TraceStart).GetSafeNormal();

		//If the actor hit is a character, then apply damage and spawn a hit effect where the shot landed...
		if (Target->Implements<UOPCharacterInterface>())
		{
			UGameplayStatics::ApplyPointDamage(Target, Stats.Damage, ShotFromDirection, WeaponHitResult, GetOwner()->GetInstigatorController(), this, Stats.DamageType);

			if (IsValid(CharacterHitEffect)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, CharacterHitEffect, WeaponHitResult.ImpactPoint);
		}
		//...Otherwise, just spawn a different hit effect.
		else
		{
			if (IsValid(EnvironmentHitEffect)) UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, EnvironmentHitEffect, WeaponHitResult.ImpactPoint);
		}
	}
}

void AOPWeapon::EndFiringCooldown()
{
	bFiringCooldownActive = false;
}

void AOPWeapon::EndAnimationCooldown()
{
	bAnimationCooldownActive = false;
}

void AOPWeapon::CheckInfiniteAmmoStatus()
{
	//Need to get a reference to the controller of the weapon's owner.
	TObjectPtr<AController> Controller = GetOwner()->GetInstigatorController();
	
	//Only the player is allowed to have infinite ammo.
	if (IsValid(Controller) && Controller->IsPlayerController())
	{
		//If infinite ammo is enabled, then no ammo will be subtracted from the weapon's magazine.
		if (WorldSubsystem->bInfiniteAmmoEnabled) return;
		
		//If infinite ammo with reloading is enabled, then an event will be called that replenishes the player's reserve ammo.
		if (WorldSubsystem->bInfiniteAmmoWithReloadEnabled) WorldSubsystem->OnInfiniteAmmoWithReloadUpdate.Broadcast(Stats.WeaponType);
	}
	
	Stats.CurrentMagazine--;
}

void AOPWeapon::StartFocus_Implementation()
{
	
}

void AOPWeapon::EndFocus_Implementation()
{
	
}

void AOPWeapon::OnInteract_Implementation(AActor* CallingPlayer)
{
	//Execute weapon pick-up logic on the player.
	if (CallingPlayer->Implements<UOPCharacterInterface>()) IOPCharacterInterface::Execute_PickUpWeapon(CallingPlayer, this);
}

FText AOPWeapon::GetInteractableObjectName_Implementation()
{
	return Stats.WeaponName;
}

EInteractType AOPWeapon::GetInteractableObjectType_Implementation()
{
	return Stats.ObjectType;
}