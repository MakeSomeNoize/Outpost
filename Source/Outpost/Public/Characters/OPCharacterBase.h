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

	/* Character health */

	//Returns the character's current health.
	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "OPCharacterBase|Health")
		FORCEINLINE int32 GetCurrentHealth() { return CurrentHealth; }

	/*
	Changes the character's current health.
	@param	NewValue	The value that CurrentHealth should be changed to.
	*/
	UFUNCTION(BlueprintCallable, BlueprintSetter, Category = "OPCharacterBase|Health")
		void SetCurrentHealth(int32 NewValue);

	//Returns the character's current health.
	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "OPCharacterBase|Health")
		FORCEINLINE int32 GetMaxHealth() { return MaxHealth; }

	/*
	Changes the character's current health.
	@param	NewValue	The value that MaxHealth should be changed to.
	*/
	UFUNCTION(BlueprintCallable, BlueprintSetter, Category = "OPCharacterBase|Health")
		void SetMaxHealth(int32 NewValue);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Character health */

	//The amount of health that the character currently has.
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter = GetCurrentHealth, BlueprintSetter = SetCurrentHealth, Category = "OPCharacterBase|Health")
		int32 CurrentHealth = 1;

	//The maximum amount of health that the character can have.
	UPROPERTY(VisibleInstanceOnly, BlueprintGetter = GetMaxHealth, BlueprintSetter = SetMaxHealth, Category = "OPCharacterBase|Health")
		int32 MaxHealth = 100;

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