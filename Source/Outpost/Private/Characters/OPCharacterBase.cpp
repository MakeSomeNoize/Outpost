// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPCharacterBase.h"

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
	
}

// Called every frame
void AOPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

const TArray<FHitResult> AOPCharacterBase::MeleeSphereTrace_Implementation(FVector MeleeStart, FVector MeleeEnd, float Radius)
{
	//Using an empty array as a placeholder for now.
	return TArray<FHitResult>();
}

void AOPCharacterBase::CharacterDeath()
{
	//Detach the character from their controller.
	DetachFromControllerPendingDestroy();

	bIsCharacterDead = true;
}
