// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OPCharacterBase.h"
#include "InputActionValue.h"
#include "Components/TimelineComponent.h"
#include "OPPlayer.generated.h"

//Forward declarations.
class UInputAction;
class UInputMappingContext;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class OUTPOST_API AOPPlayer : public AOPCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPPlayer(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Required for EnhancedInput plugin.
	virtual void PawnClientRestart() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};