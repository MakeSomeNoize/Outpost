// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Subsystems/OPWorldSubsystem.h"
#include "Interfaces/OPCharacterInterface.h"
#include "OPCharacterBase.generated.h"

UCLASS()
class OUTPOST_API AOPCharacterBase : public ACharacter, public IOPCharacterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPCharacterBase(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Overridden from OPCharacterInterface */

	virtual const TArray<FHitResult> MeleeSphereTrace_Implementation(FVector MeleeStart, FVector MeleeEnd, float Radius) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* General booleans */

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "OPCharacterBase|General Booleans")
		bool bIsCharacterDead;

	//An array of all the actors that were hit by the character's last melee attack.
	UPROPERTY(BlueprintReadWrite, Category = "OPCharacterBase")
		TArray<TObjectPtr<AActor>> ActorsHit;
	
	UPROPERTY()
		TObjectPtr<UOPWorldSubsystem> WorldSubsystem;

	virtual void CharacterDeath();
};