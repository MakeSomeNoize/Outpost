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
	
}

// Called every frame
void AOPCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}