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

	//Fire as many shots as the weapon is meant to. Unless the weapon is a shotgun, this loop should only iterate once.
	for (int i = 0; i < Stats.ShotAmount; i++)
	{
		WeaponLineTrace();
	}
	
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
	FVector ShotAngle = FMath::VRandCone(CameraRotation.Vector(), Stats.SpreadRadius, Stats.SpreadRadius);

	return CameraLocation + ShotAngle * Stats.MaxRange;
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
