// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/OPCharacterBase.h"
#include "OPEnemy.generated.h"

//Forward declarations.
class UUASAimAssistTargetComponent;

/**
 * 
 */
UCLASS()
class OUTPOST_API AOPEnemy : public AOPCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOPEnemy(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Actor and scene components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OPEnemy|Components")
		TObjectPtr<UUASAimAssistTargetComponent> AimAssistTargetComponent;

	/* Overriden from OPCharacterInterface */

	virtual void UpdateLastHitMaterial_Implementation(UPhysicalMaterial* MaterialHit) override;

	//Overridden from OPCharacterBase class.
	virtual void CharacterDeath() override;

	/* Character materials */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FCharacterMaterials DamageMaterials;

	/* Death and respawning */
	
	//The amount of time it takes for a dead enemy's body to be cleared from the level.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OPEnemy|Death and Respawning")
		float ClearTimer = 30.f;

	UFUNCTION()
		void TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser);

	void ClearEnemy();

	//Used for calculating location-based damage.
	TObjectPtr<UPhysicalMaterial> LastHitMaterial;

	FTimerHandle ClearHandle;
};