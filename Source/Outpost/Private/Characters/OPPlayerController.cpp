// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/OPPlayerController.h"

// Sets default values
AOPPlayerController::AOPPlayerController()
{
	// Set this player controller to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOPPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}