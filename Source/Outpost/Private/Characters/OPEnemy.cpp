// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPEnemy.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UASAimAssistTargetComponent.h"

// Sets default values
AOPEnemy::AOPEnemy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AimAssistTargetComponent = CreateDefaultSubobject<UUASAimAssistTargetComponent>("Aim Assist Target Component");
}

// Called when the game starts or when spawned
void AOPEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Add the enemy to the global enemy array, as soon as they spawn.
	if (IsValid(WorldSubsystem)) WorldSubsystem->EnemyArray.Emplace(this);

	//Bind a callback function to OnTakePointDamage delegate.
	OnTakePointDamage.AddDynamic(this, &AOPEnemy::TakePointDamage);

	//Initialize the enemy as an aim assist target.
	AimAssistTargetComponent->Init(GetMesh());
}

// Called every frame
void AOPEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AOPEnemy::CharacterDeath()
{
	if (bIsCharacterDead) return;

	Super::CharacterDeath();

	//Remove the enemy from the global enemy array once they die, and update enemy information.
	if (IsValid(WorldSubsystem))
	{
		WorldSubsystem->EnemyArray.Remove(this);
		WorldSubsystem->OnEnemyUpdate.Broadcast();
	}

	//The enemy goes into a ragdoll state.
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");

	//Set a timer for when the enemy's body will be cleared from the level.
	GetWorldTimerManager().SetTimer(ClearHandle, this, &AOPEnemy::ClearEnemy, ClearTimer);
}

void AOPEnemy::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	int32 FinalDamage;
	
	//Damage dealt to the enemy will be determined by the type of physical material that was hit. 
	if (LastHitMaterial == DamageMaterials.HeadMaterial)
	{
		FinalDamage = Damage * 2;
	}
	else if (LastHitMaterial == DamageMaterials.LimbMaterial)
	{
		FinalDamage = Damage * 0.8;
	}
	else
	{
		FinalDamage = Damage;
	}
	
	//The character cannot have a health value below 0.
	CurrentHealth = FMath::Clamp((CurrentHealth - FinalDamage), 0, MaxHealth);

	//If the character has run out of health, then they die.
	if (CurrentHealth <= 0)
	{
		CharacterDeath();
	}
}

void AOPEnemy::ClearEnemy()
{
	//The enemy's body is cleared from the level, after a specified amount of time.
	Destroy();
}

void AOPEnemy::UpdateLastHitMaterial_Implementation(UPhysicalMaterial* MaterialHit)
{
	LastHitMaterial = MaterialHit;
}