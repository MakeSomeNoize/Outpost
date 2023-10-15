// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPCharacterBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AOPCharacterBase::AOPCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOPCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	//Get a reference to the world subsystem.
	WorldSubsystem = GetWorld()->GetSubsystem<UOPWorldSubsystem>();

	SetCurrentHealth(MaxHealth);
}

// Called every frame
void AOPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AOPCharacterBase::SetCurrentHealth(int32 NewValue)
{
	//The character's health should never go below 0, or above their max health.
	CurrentHealth = FMath::Clamp(NewValue, 0, MaxHealth);

	//BROADCAST TO "HEALTH UPDATE" DELEGATE HERE
}

void AOPCharacterBase::SetMaxHealth(int32 NewValue)
{
	//The character's max health should never go below what their current health is.
	if (NewValue >= CurrentHealth) MaxHealth = NewValue;

	//BROADCAST TO "HEALTH UPDATE" DELEGATE HERE
}

void AOPCharacterBase::CharacterDeath()
{
	//Detach the character from their controller.
	DetachFromControllerPendingDestroy();

	bIsCharacterDead = true;
}

void AOPCharacterBase::ProcessMeleeHitOnTargets_Implementation()
{
	for (FHitResult Index : MeleeHitResults)
	{
		if (IsValid(Index.GetActor()))
		{
			//Make sure that none of the actors hit have already been damaged by this melee attack.
			if (!ActorsDamaged.Contains(Index.GetActor()))
			{
				UGameplayStatics::ApplyDamage(Index.GetActor(), MeleeDamage, GetController(), this, MeleeDamageType);
				ActorsDamaged.Emplace(Index.GetActor());
			}
		}
	}

	//Clear out the array of actors damaged once we're done.
	ActorsDamaged.Empty();
}