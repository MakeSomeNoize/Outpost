// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OUTPOST_API AOPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPPlayerController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};