// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPCharacterBase.h"
#include "Kismet/KismetSystemLibrary.h"

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

TArray<FHitResult> AOPCharacterBase::MeleeSphereTrace_Implementation(FVector MeleeStart, FVector MeleeEnd, float Radius)
{
	//LOGIC FOR PERFORMING A SPHERE TRACE GOES HERE
	
	//Using an empty array as a placeholder for now.
	return TArray<FHitResult>();
}