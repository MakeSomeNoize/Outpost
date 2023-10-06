// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/OPWeapon.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/OPWorldSubsystem.h"
#include "Components/BoxComponent.h"

// Sets default values
AOPWeapon::AOPWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponRoot = CreateDefaultSubobject<USceneComponent>("Weapon Root");
	RootComponent = WeaponRoot;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("Weapon Mesh");
	WeaponMesh->SetupAttachment(WeaponRoot);

	InteractRadius = CreateDefaultSubobject<UBoxComponent>("Interact Radius");
	InteractRadius->SetupAttachment(WeaponRoot);
	
}

// Called when the game starts or when spawned
void AOPWeapon::BeginPlay()
{
	Super::BeginPlay();

	//Get a reference to the world subsystem.
	WorldSubsystem = GetWorld()->GetSubsystem<UOPWorldSubsystem>();

	CurrentMagazine = MaxMagazine;
}

// Called every frame
void AOPWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOPWeapon::Shoot()
{
	//The weapon cannot shoot, if its magazine is empty.
	if (CurrentMagazine <= 0) return;

	//If the weapon is a shotgun, then all of its shots will fire at once.
	if (WeaponType == EWeaponType::Shotgun)
	{
		//LOGIC FOR PLAYING FIRING EFFECT AND FIRING SOUND GO HERE

		for (int i = 0; i < ShotAmount; i++)
		{
			WeaponLineTrace();
		}
	}
	//If the weapon is burst-fire, then its shots will fire in sequence.
	else if (WeaponTags.HasTagExact(FGameplayTag::RequestGameplayTag("Weapon.IsBurst")) && BurstCount < ShotAmount)
	{
		//LOGIC FOR PLAYING FIRING EFFECT AND FIRING SOUND GO HERE

		//BURST-FIRE LOGIC GOES HERE
	}
	//Otherwise, only one shot will be fired.
	else
	{
		//LOGIC FOR PLAYING FIRING EFFECT AND FIRING SOUND GO HERE

		WeaponLineTrace();
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
		if (IsValid(WorldSubsystem) && WorldSubsystem->bDebugLinesEnabled)
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, WeaponHitResult, true, FLinearColor::Red, FLinearColor::Green, 2.5f);
		}
		else
		{
			UKismetSystemLibrary::LineTraceSingle(this, CameraLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::None, WeaponHitResult, true, FLinearColor::Red, FLinearColor::Green, 0.f);
		}
	}
}

FVector AOPWeapon::CalculateWeaponSpread()
{
	//The angle of the shot is randomly generated within a cone-shaped area.
	FVector ShotAngle = FMath::VRandCone(CameraRotation.Vector(), SpreadRadius, SpreadRadius);

	return CameraLocation + ShotAngle * MaxRange;
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
		if (WorldSubsystem->bInfiniteAmmoWithReloadEnabled) WorldSubsystem->OnInfiniteAmmoWithReloadUpdate.Broadcast(WeaponType);
	}
	
	CurrentMagazine--;
}

void AOPWeapon::StartFocus_Implementation()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPWeapon::EndFocus_Implementation()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

void AOPWeapon::OnInteract_Implementation(AActor* CallingPlayer)
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s called"), *(FString(__FUNCTION__)))); //FOR TESTING ONLY
}

FText AOPWeapon::GetInteractMessage_Implementation()
{
	return InteractMessage;
}
